grid wire 0.000000 (direction x) (offset 0.000000)
grid wire 0.000000 (direction Y) (offset 0.000000)
grid via 0.000000 (direction x) (offset 0.000000)
grid via 0.000000 (direction Y) (offset 0.000000)
set route_diagonal 0
cost via -1
limit outside -1
select component U13 
fanout 1 (direction in_out) (location anywhere) (max_len -1) (smart_via_grid two_wire_between forced) (pin_type all) (share_len -1)
write routes $/SpecctraWithinLayout.rte
quit -c