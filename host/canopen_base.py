from canopen import RemoteNode, Network, objectdictionary
from typing import List
import time

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
        time.sleep(0.05)
        if verbose:
            for node_id in self.network.scanner.nodes:
                print("Found node %d!" % node_id)
        return self.network.scanner.nodes

    def __del__(self):
        self.network.disconnect()

def list_objectdict(nid=127, show_value=False, eds='ASDA_A2_1042sub980_C.eds'):
    cos = CanOpenStack()
    node = cos.add_node(nid, eds)

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
                    
def discovery():
    cos = CanOpenStack()
    cos.scan_network(verbose=True)

def dump(node, *names, converter = hex):
    print('  '.join(
        [f'{node.sdo[name].name}: {converter(node.sdo[name].read())}[{bin(node.sdo[name].read())}]' for name in names])
          )
