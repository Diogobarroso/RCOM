configure terminal
vlan 60
end


configure terminal
vlan 61
end


configure terminal
interface fastethernet 0/13
switchport mode access
switchport access vlan 60
end


configure terminal
interface fastethernet 0/17
switchport mode access
switchport access vlan 60
end


configure terminal
interface fastethernet 0/15
switchport mode access
switchport access vlan 61
end


configure terminal
interface fastethernet 0/19
switchport mode access
switchport access vlan 61
end
