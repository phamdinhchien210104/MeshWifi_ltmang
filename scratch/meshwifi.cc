#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mesh-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/animation-interface.h"
#include <fstream>
#include <iomanip>

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("MeshNetworkNetAnim");

struct FlowResult {
    uint32_t caseId, nodes; double distance;
    std::string src, dst; uint64_t txPackets, rxPackets, lostPackets;
    double lossPercent, delayUs, jitterMs, throughputKbps; bool found;
};

static FlowResult RunScenario(uint32_t caseId, uint32_t nMeshNodes, double distance, 
                               double simTime, bool enableAnim, uint32_t packetSize, 
                               double interval, uint32_t maxPackets) {
    FlowResult result = {caseId, nMeshNodes, distance, "", "", 0, 0, 0, 0, 0, 0, 0, false};
    
    NodeContainer meshNodes; meshNodes.Create(nMeshNodes);
    
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(0.0),
        "MinY", DoubleValue(0.0), "DeltaX", DoubleValue(distance), "DeltaY", DoubleValue(distance),
        "GridWidth", UintegerValue(3), "LayoutType", StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(meshNodes);
    
    if (nMeshNodes > 1) {
        Ptr<ConstantPositionMobilityModel> mobM1 = meshNodes.Get(1)->GetObject<ConstantPositionMobilityModel>();
        if (mobM1) mobM1->SetPosition(Vector(50.0, 0.0, 0.0));
    }
    
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper wifiPhy; 
    wifiPhy.SetChannel(wifiChannel.Create());
    // Bật PCAP để debug nếu cần
    // wifiPhy.EnablePcapAll("mesh-pcap");
    
    MeshHelper mesh = MeshHelper::Default();
    mesh.SetStackInstaller("ns3::Dot11sStack");
    NetDeviceContainer meshDevices = mesh.Install(wifiPhy, meshNodes);
    
    InternetStackHelper internet; internet.Install(meshNodes);
    Ipv4AddressHelper ipv4; ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer meshInterfaces = ipv4.Assign(meshDevices);
    
    uint16_t port = 9;
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApp = echoServer.Install(meshNodes.Get(nMeshNodes - 1));
    serverApp.Start(Seconds(1.0)); serverApp.Stop(Seconds(simTime));
    
    UdpEchoClientHelper echoClient(meshInterfaces.GetAddress(nMeshNodes - 1), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(maxPackets));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(interval)));
    echoClient.SetAttribute("PacketSize", UintegerValue(packetSize));
    ApplicationContainer clientApp = echoClient.Install(meshNodes.Get(0));
    clientApp.Start(Seconds(2.0)); clientApp.Stop(Seconds(simTime - 2.0));
    
    // ========== PHẦN QUAN TRỌNG CHO NETANIM ==========
    AnimationInterface *anim = nullptr;
    if (enableAnim) {
        std::ostringstream animName;
        animName << "mesh-" << nMeshNodes << "-" << caseId << "-d" << (int)distance << ".xml";
        anim = new AnimationInterface(animName.str().c_str());
        
        // 1. Bật ghi nhận packet metadata (QUAN TRỌNG NHẤT!)
        anim->EnablePacketMetadata(true);
        
        // 2. Thiết lập tốc độ cập nhật vị trí
        anim->SetMobilityPollInterval(Seconds(0.1));
        
        // 3. Giới hạn số packet ghi (tránh file quá lớn)
        anim->SetMaxPktsPerTraceFile(500000);
        
        // 4. Bật IP/MAC tracking để thấy địa chỉ trong NetAnim
        anim->EnableIpv4L3ProtocolCounters(Seconds(0), Seconds(simTime));
        
        // 5. Cấu hình node appearance
        for (uint32_t i = 0; i < nMeshNodes; i++) {
            std::ostringstream nodeDesc;
            double size = 4.0;
            uint8_t r = 0, g = 200, b = 0;
            
            if (i == 0) {
                nodeDesc << "CLIENT\n" << meshInterfaces.GetAddress(0);
                r = 255; g = 0; b = 0; size = 6.0;
            } else if (i == nMeshNodes - 1) {
                nodeDesc << "SERVER\n" << meshInterfaces.GetAddress(nMeshNodes - 1);
                r = 0; g = 0; b = 255; size = 6.0;
            } else {
                nodeDesc << "M" << i << "\n" << meshInterfaces.GetAddress(i);
                r = 0; g = 200; b = 0; size = 5.0;
            }
            
            anim->UpdateNodeDescription(meshNodes.Get(i), nodeDesc.str());
            anim->UpdateNodeColor(meshNodes.Get(i), r, g, b);
            anim->UpdateNodeSize(meshNodes.Get(i)->GetId(), size, size);
        }
        
        std::cout << "NetAnim file: " << animName.str() << "\n";
    }
    // ================================================
    
    FlowMonitorHelper flowHelper; 
    Ptr<FlowMonitor> flowMonitor = flowHelper.InstallAll();
    
    Simulator::Stop(Seconds(simTime)); 
    Simulator::Run();
    
    flowMonitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats();
    Ipv4Address clientAddr = meshInterfaces.GetAddress(0);
    Ipv4Address serverAddr = meshInterfaces.GetAddress(nMeshNodes - 1);
    
    for (auto &entry : stats) {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(entry.first);
        if (t.sourceAddress == clientAddr && t.destinationAddress == serverAddr) {
            const FlowMonitor::FlowStats &f = entry.second;
            uint32_t lostPackets = f.txPackets - f.rxPackets;
            double lossRate = (f.txPackets > 0) ? (lostPackets * 100.0) / f.txPackets : 0.0;
            double delayUs = (f.rxPackets > 0) ? f.delaySum.GetMicroSeconds() / (double)f.rxPackets : 0.0;
            double jitterMs = (f.rxPackets > 1) ? f.jitterSum.GetMilliSeconds() / (double)(f.rxPackets - 1) : 0.0;
            double throughputKbps = 0.0;
            double durationUs = f.timeLastRxPacket.GetMicroSeconds() - f.timeFirstTxPacket.GetMicroSeconds();
            if (durationUs > 0 && f.rxBytes > 0) {
                throughputKbps = (f.rxBytes * 8.0) / ((durationUs / 1e6) * 1e3);
            }
            
            std::ostringstream ossSrc, ossDst;
            ossSrc << t.sourceAddress; ossDst << t.destinationAddress;
            result = {caseId, nMeshNodes, distance, ossSrc.str(), ossDst.str(),
                     f.txPackets, f.rxPackets, lostPackets, lossRate, delayUs, jitterMs, throughputKbps, true};
            break;
        }
    }
    
    if (anim) delete anim;
    Simulator::Destroy();
    return result;
}

int main(int argc, char **argv) {
    // QUAN TRỌNG: Bật PacketMetadata TRƯỚC KHI tạo bất kỳ node nào
    PacketMetadata::Enable();
    
    uint32_t nMeshNodes = 5, startNodes = 0, endNodes = 0, packetSize = 1024, maxPackets = 10;
    double distance = 20.0, simTime = 20.0, interval = 1.0, repeatDistance = 0.0;
    double startDistance = 0.0, endDistance = 0.0, stepDistance = 0.0;
    bool enableAnim = true;
    
    CommandLine cmd;
    cmd.AddValue("nodes", "So nodes", nMeshNodes);
    cmd.AddValue("distance", "Khoang cach (m)", distance);
    cmd.AddValue("time", "Thoi gian (s)", simTime);
    cmd.AddValue("anim", "NetAnim", enableAnim);
    cmd.AddValue("packets", "So goi", maxPackets);
    cmd.AddValue("interval", "Interval (s)", interval);
    cmd.AddValue("size", "Packet size", packetSize);
    cmd.AddValue("startNodes", "Start nodes", startNodes);
    cmd.AddValue("endNodes", "End nodes", endNodes);
    cmd.AddValue("repeatDistance", "Repeat distance", repeatDistance);
    cmd.AddValue("startDistance", "Start distance", startDistance);
    cmd.AddValue("endDistance", "End distance", endDistance);
    cmd.AddValue("stepDistance", "Step distance", stepDistance);
    cmd.Parse(argc, argv);
    
    std::cout << "Xoa cac file XML cu...\n";
    (void)system("rm -f mesh-*.xml");
    
    std::vector<double> distancesToRun;
    if (startDistance > 0 && endDistance >= startDistance && stepDistance > 0) {
        for (double d = startDistance; d <= endDistance + 1e-9; d += stepDistance)
            distancesToRun.push_back(d);
    } else {
        distancesToRun.push_back(distance);
        if (repeatDistance > 0 && std::abs(repeatDistance - distance) > 1e-9)
            distancesToRun.push_back(repeatDistance);
    }
    
    std::vector<FlowResult> allResults;
    uint32_t nextCaseId = 1;
    
    if (startNodes > 0 && endNodes >= startNodes) {
        for (double dist : distancesToRun) {
            for (uint32_t nn = startNodes; nn <= endNodes; ++nn) {
                allResults.push_back(RunScenario(nextCaseId++, nn, dist, simTime, enableAnim, packetSize, interval, maxPackets));
            }
        }
        
        std::cout << "\n=== BANG SO SANH ===\n";
        std::cout << std::left << std::setw(6) << "Case" << std::setw(8) << "Nodes"
                  << std::setw(8) << "Dist" << std::setw(8) << "TX" << std::setw(8) << "RX"
                  << std::setw(10) << "Lost" << std::setw(10) << "Loss(%)"
                  << std::setw(12) << "Delay(us)" << std::setw(12) << "Jitter(ms)"
                  << std::setw(14) << "Thr(kbps)\n" << std::string(100, '-') << "\n";
        
        for (const auto &r : allResults) {
            std::cout << std::left << std::setw(6) << r.caseId << std::setw(8) << r.nodes
                      << std::setw(8) << (int)r.distance
                      << std::setw(8) << (r.found ? r.txPackets : 0)
                      << std::setw(8) << (r.found ? r.rxPackets : 0)
                      << std::setw(10) << (r.found ? r.lostPackets : 0)
                      << std::setw(10) << std::fixed << std::setprecision(2) << (r.found ? r.lossPercent : 0)
                      << std::setw(12) << std::setprecision(0) << (r.found ? r.delayUs : 0)
                      << std::setw(12) << std::fixed << std::setprecision(5) << (r.found ? r.jitterMs : 0)
                      << std::setw(14) << std::setprecision(3) << (r.found ? r.throughputKbps : 0) << "\n";
        }
    } else {
        for (double dist : distancesToRun)
            allResults.push_back(RunScenario(nextCaseId++, nMeshNodes, dist, simTime, enableAnim, packetSize, interval, maxPackets));
    }
    
    std::cout << "\nHoan tat!\n";
    return 0;
}