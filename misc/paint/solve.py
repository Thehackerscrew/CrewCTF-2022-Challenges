import pygame
from scapy.packet import Raw
from scapy.utils import *
from scapy.layers.usb import USBpcap
from struct import unpack

w, h = 800, 500

queue = []

fn = 'paint.pcap'
start = next(PcapReader(fn)).time

def parse_hid(raw):
    try:
        _, x, y, z, _, buttons = unpack('cBBBBc', raw[:6])
    except struct.error:
        return False, 0, 0, 0,

    # The X button is used when drawing, as should become evident when analyzing the packets
    down = buttons[0] & 0b00100000
    x, y, z = x - 128, y - 128, z
    return down, x, y, z

for pkt in PcapReader(fn):
    try:
        by = pkt[USBpcap][Raw].load
    except IndexError:
        continue
    down, x, y, z = parse_hid(by)

    # Account for slight controller drift (not neccesary)
    if abs(x) + abs(y) < 10: x = y = 0

    queue.append({
        'time': pkt.time - start,
        'x': x,
        'y': y,
        'down': down
        })

screen = pygame.display.set_mode( (w, h) )
x, y, down = 0, 0, False
pos = [0, h // 2]

start = time.time()
while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            quit()

    t = time.time() - start

    while True and len(queue) > 0:
        q = queue.pop(0)
        x = q['x']
        y = q['y']
        down = q['down']
        if q['time'] > t:
            break

    # Scale it down so it's visible
    pos[0] += x / 500
    pos[1] += y / 500

    if down:
        pygame.draw.circle( screen, (0, 0, 255), ( pos[0], pos[1]), 5 )

    pygame.display.flip()