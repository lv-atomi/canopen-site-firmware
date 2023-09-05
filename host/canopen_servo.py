from canopen import RemoteNode, Network, objectdictionary
from typing import List
import time
from canopen_base import CanOpenStack, discovery, dump
import logging

# 配置logging模块
#logging.basicConfig(level=logging.DEBUG)




def init_delta_servo(nid=124):
    cos = CanOpenStack()
    node = cos.add_node(nid, 'ASDA_A2_1042sub980_C.eds')
    # clear function mapping for all Digital Input and Extended Digital Inputs
    for name in ['P2-10', 'P2-11', 'P2-12', 'P2-13', 'P2-14', 'P2-15', 'P2-16', 'P2-17',
                 'P2-36', 'P2-37', 'P2-38', 'P2-39', 'P2-40', 'P2-41',
                 'P0-45',       # Status Monitor Register Selection, default 0
                 
                 ]:
        node.sdo[name].write(0)

    node.sdo['P2-10'].write(1)   # servo enable
    node.sdo['P1-32'].write(16)  # Motor Stop Mode
    node.sdo['P1-44'].write(1)   # gear ratio
    node.sdo['P1-45'].write(1)   # gear ratio
    node.sdo['P1-55'].write(5000)   # max speed limit
    node.sdo['P3-10'].write(1)      # CANopen / DMCNET Protocol Setting  ???
    node.sdo['P5-11'].write(2)     # Data Array - Address of Reading / Writing   ???

    node.sdo[0x605b].write(0)   # Shutdown option code, 0->Disable drive function
    node.sdo[0x6081].write(10000) # profile velocity
    node.sdo[0x6085].write(200)   # quick stop deceleration
    node.sdo[0x60f2].write(0)     # Positioning option code
    node.sdo['P1-01'].write(0xc)    
    
    dump(node, 'P2-10', 'P2-11', 'P2-12', 'P2-13', 'P2-14', 'P2-15', 'P2-16', 'P2-17')
    dump(node, 'P2-36', 'P2-37', 'P2-38', 'P2-39', 'P2-40', 'P2-41')


def test_delta_drive_sdo(nid=124):
    cos = CanOpenStack()
    node = cos.add_node(nid, 'ASDA_A2_1042sub980_C.eds')

    # set operation mode
    data = node.sdo[0x6060]     # modes of operation
    print(f"{data.od.name}[{hex(data.od.index)}]: {data.data} {data.read()}")

    mode_of_operation = node.sdo['Modes of operation']
    print("Modes of operation set to:", 3, "   # 3:Profile velocity mode")
    mode_of_operation.write(3)  # velocity mode


    # check status
    dump(node, 'Statusword', 'Controlword', 'P1-01')

    node.sdo['Fault reaction option code'].write(2)
    print("Fault reaction option code set to:", 2, "   # 2:slow down on quick stop ramp")
    dump(node, 'Statusword', 'Controlword', 'P1-01', 0x6060)
    
    node.sdo['Controlword'].bits[7]=1 # fault reset
    print("Controlword bit 7 set to:", 1, "   # The rising edge of Bit 7 is used to reset a fault")

    node.sdo['P1-01'].write(0xc)
    node.sdo[0x60C0].write(0)
    print("P1-01[6060h] set to:", 0x1, "   #0xc => full can mode")
    dump(node, 'Statusword', 'Controlword', 'P1-01', 0x60c0)

    cw = node.sdo['Controlword']
    cw.write(0x6)               # shutdown
    print("Controlword set to 0b110   # fire shutdown event")
    dump(node, 'Statusword', 'Controlword')

    node.sdo['Profile acceleration'].write(1000)
    node.sdo['Profile deceleration'].write(1000)
    node.sdo['Target velocity'].write(1000) # 100 rpm
    print("set acceleration,deceleration profile & target velocity...")

    cw.write(0x7)               # switch on
    print("Controlword set to 0b111   # fire switch on event")
    dump(node, 'Statusword', 'Controlword')

    cw.write(0xf)               # enable operation
    print("Controlword set to 0b1111   # fire enable operation event")
    dump(node, 'Statusword', 'Controlword')

    while True:
        dump(node, 'Statusword', 'Controlword', 'Target velocity', 'P0-46', 'P4-00', 'P4-06', 'P3-06')
        time.sleep(0.5)
        node.sdo['Target velocity'].write(1000)


def test_disable_drive(nid=127):
    cos = CanOpenStack()
    node = cos.add_node(nid, 'ASDA_A2_1042sub980_C.eds')
    node.sdo['Controlword'].write(0)


def test_servo_position_mode(nid=124):
    cos = CanOpenStack()
    node = cos.add_node(nid, 'ASDA_A2_1042sub980_C.eds')

    node.sdo['Controlword'].write(0)

    # node.sdo['P1-44'].write(128)   # gear ratio
    # node.sdo['P1-45'].write(1)   # gear ratio
    
    print('6093.1:', node.sdo[0x6093][1].read())
    print('6093.2:', node.sdo[0x6093][2].read())
    # set operation mode
    data = node.sdo[0x6060]     # modes of operation
    print(f"{data.od.name}[{hex(data.od.index)}]: {data.data} {data.read()}")
    
    mode_of_operation = node.sdo['Modes of operation']
    print("Modes of operation set to:", 1, "   # 1:position mode")
    mode_of_operation.write(1)  # position mode

    node.sdo[0x60C0].write(0) # Host does not send [60C1h Sub-3]. It could save calculating time of host and Drive could work also. 
        
    print("P1-01[6060h] set to:", 0xc, "   #0xc => full can mode")
    dump(node, 'Statusword', 'Controlword', 'P1-01', 0x60c0)
    
    cw = node.sdo['Controlword']
    cw.write(0x6)               # shutdown
    
    node.sdo['Target Position'].write(0)
    node.sdo['Profile velocity'].write(10000) # 100 rpm
    node.sdo['Profile acceleration'].write(1000)
    node.sdo['Profile deceleration'].write(1000)
    
    cw.write(0x7)               # switch on
    cw.write(0xf)               # enable operation
    cw.write(0x1f)              # send position

    c = node.sdo[0x6064].read()
    while True:
        c += 1000
        # dump(node, 'Modes of operation', 'Statusword', 'Controlword', 'Target Position', 0x6081,
        #      0x6061, 0x6062, 0x6063, 0x6064, 0x60f4)
        time.sleep(1)
        node.sdo['Target Position'].write(c)
        # position will sent on raising edge of bit5, send 0b01111, then 0b11111
        cw.write(0xf)
        cw.write(0x1f)

def test_servo_position_mode_pdo(nid=124):
    cos = CanOpenStack()
    node = cos.add_node(nid, 'ASDA_A2_1042sub980_C.eds')
    node.sdo['Controlword'].write(0)
    
    # set operation mode
    data = node.sdo[0x6060]     # modes of operation
    print(f"{data.od.name}[{hex(data.od.index)}]: {data.data} {data.read()}")

   
    mode_of_operation = node.sdo['Modes of operation']
    print("Modes of operation set to:", 7, "   # 7:interpolated position mode")
    mode_of_operation.write(7)  # interpolated position mode

    node.sdo[0x60C0].write(0) # Host does not send [60C1h Sub-3]. It could save calculating time of host and Drive could work also. 
        
    # 设置通信周期
    node.sdo[0x1006].write(10000)  # 10000微秒，即10毫秒

    cos.network.sync.start(0.1)
    # 设置插补时间周期
    # node.sdo[0x60C2][1].write(1)  # 1毫秒
    # node.sdo[0x60C2][2].write(-3)  # 插补时间单位为10^-3秒

    # 配置RPDO映射
    node.rpdo[1].clear()
    node.rpdo[1].cob_id = 0x200 + 124
    node.rpdo[1].trans_type = 1  # 设置传输类型
    node.rpdo[1].event_timer = 1000  # 设置事件定时器
    node.rpdo[1].add_variable("Interpolation data record", "Parameter1 of ip function")
    node.rpdo[1].add_variable(0x6040)
    node.rpdo[1].enabled = True

    # 设置RPOD通信参数
    node.nmt.state = 'PRE-OPERATIONAL'
    
    
    # try:
    #     node.sdo[0x1400][1].write(0x200 + nid)  # COB-ID, 这里只是示例值
    # except:
    #     pass
    print("1400h.1:", node.sdo[0x1400][1].read())
    # node.sdo[0x1400][1].write(0x01)  # 传输类型, 这里只是示例值

    # 保存新配置 (节点必须处于预操作状态)
    node.rpdo[1].save()

    c = node.sdo[0x6064].read() # read current position
    node.rpdo[1][0x6040].raw = 0x7
    node.rpdo[1][0x6040].raw = 0xf
    # 启动RPDO1传输，间隔100ms
    node.rpdo[1]["Interpolation data record.Parameter1 of ip function"].raw = c  # 例如，设置目标位置为0x100000
    node.rpdo[1][0x6040].raw = 0x1F  # 例如，设置控制字为0x0F
    node.rpdo[1].start(0.1)
    
    
    node.nmt.state = 'OPERATIONAL'


    while True:
        c += 10
        #dump(node, 'Modes of operation', 'Statusword', 'Controlword')
        time.sleep(.01)
        node.rpdo[1]["Interpolation data record.Parameter1 of ip function"].raw = c
        #node.sdo['Target Position'].write(1000)

def test_servo_pdo(nid=124):
    cos = CanOpenStack()
    node = cos.add_node(nid, 'ASDA_A2_1042sub980_C.eds')
    node.nmt.state = 'PRE-OPERATIONAL'
    time.sleep(1)

    node.tpdo.read()
    node.rpdo.read()
    
    
    
if __name__ == '__main__':
    #discovery()
    #test_delta_drive_sdo(124)
    test_servo_position_mode_pdo(124)
    #test_servo_position_mode(124)
    #test_servo_pdo()
    #test_disable_drive(124)
    


