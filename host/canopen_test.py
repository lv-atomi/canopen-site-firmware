from canopen import RemoteNode, Network, objectdictionary
from typing import List
import time

# node = network.add_node(127, 'delta_servo_dictionary.eds')
#local_node = canopen.LocalNode(1, '/path/to/master_dictionary.eds')
#network.add_node(local_node)
EDS_PATH = '../user/canopen/application/DS301_profile2.eds'

class CanOpenStack:
    def __init__(self, ifname='can0') -> None:
        self.network = Network()
        self.network.connect(channel=ifname, bustype='socketcan')

    def add_node(self, node_id: int, object_directory_eds: str) -> RemoteNode:
        node = self.network.add_node(node_id, object_directory_eds)
        return node
    
    def scan_network(self, verbose=False) -> List[int]:
        self.network.scanner.search()
        # We may need to wait a short while here to allow all nodes to respond
        time.sleep(1.05)
        if verbose:
            for node_id in self.network.scanner.nodes:
                print("Found node %d!" % node_id)
        return self.network.scanner.nodes

    def __del__(self):
        self.network.disconnect()


def test_discovery():
    cos = CanOpenStack()
    cos.scan_network(verbose=True)

def test_list_objectdict(nid=127, show_value=False):
    cos = CanOpenStack()
    node = cos.add_node(nid, EDS_PATH)

    def dump_values(obj, show_value, oaccess):
        for subobj in obj.values():
            access = "RW" if subobj.writable and subobj.readable else "RO" if subobj.readable else "WO"
            subdisplay = f'  {subobj.subindex}[{access}]: {subobj.name}'
            if show_value and oaccess != "NONE":
                tobj = node.object_dictionary[obj.index][subobj.subindex]
                subdisplay += f" -> {node.sdo[tobj.index].od.read()}"
            print(subdisplay)
        
    
    for obj in node.object_dictionary.values():
        access = "NONE" if isinstance(obj, objectdictionary.Array) \
            or isinstance(obj, objectdictionary.Record) else \
            "RW" if obj.writable and obj.readable \
            else "RO" if obj.readable \
                 else "WO"
        display = f'0x{obj.index:X}[{access}]: {obj.name}'
        if isinstance(obj, objectdictionary.Record):
            print(display)
            dump_values(obj, show_value, access)
        else:
            if show_value:
                tobj = node.object_dictionary[obj.index]
                if isinstance(tobj, objectdictionary.Array):
                    dump_values(tobj, show_value, access)
                else:
                    display += f' -> {node.sdo[tobj.index].read()}'
            print(display)
                    

def dump(node, *names):
    print('  '.join(
        [f'{node.sdo[name].name}: {hex(node.sdo[name].read())}[{bin(node.sdo[name].read())}]' for name in names])
          )

def test_simple(nid=1):
    cos = CanOpenStack()
    node = cos.add_node(nid, EDS_PATH)
    #node.sdo[0x6000].write(3)
    dump(node, 0x6000)

def test_psu(nid=1):
    cos = CanOpenStack()
    node = cos.add_node(nid, EDS_PATH)
    # node.sdo[0x6004].write(100) # set voltage to 100
    
    dump(node, 0x6001)          # PSU current read
    dump(node, 0x6002)          # PSU voltage read
    dump(node, 0x6003)          # PSU current set read
    dump(node, 0x6004)          # PSU voltage set read
    dump(node, 0x6005)          # at32 temperature read
    
if __name__ == '__main__':
    test_discovery()
    #test_list_objectdict(nid=1, show_value=True)
    #test_simple()
    #test_psu()
