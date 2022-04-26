import os
import signal
import subprocess
import sys
from os import path

from colored import attr, bg, fg

peers = [
    ("127.0.0.1", 6000, True),
    ("127.0.0.1", 6001, False),
    ("127.0.0.1", 6002, False),
    ("127.0.0.1", 6003, False)
]

bin_dir = path.abspath(path.join('../', 'build', 'bin'))
server_bin = path.join(bin_dir, 'Server')
print(server_bin)

color = fg('white')
reset = attr('reset')

colors = [
    fg('light_green_2'),
    fg('purple_1b'),
    fg('red_3a'),
    fg('sky_blue_2')
]

print(color + "hello terminal" + reset)


def generate_env(peer):
    other_peers = map(lambda x: x[0] + ":" + str(x[1]) + ":" + ("p" if x[2] else "s"), filter(lambda x: x[1] != peer[1], peers))
    peers_arg = ','.join(other_peers)

    d = {
        # **os.environ,
        "PEERS": peers_arg,
        "BIND_IP": "0.0.0.0",
        "BIND_PORT": str(peer[1]),
        "PRIMARY": "true" if peer[2] else "false"
    }

    return d

def main():
    procs = []

    try:
        for peer in peers:
            print("Lauching " + ("primary" if peer[2] else "secondary") + " peer at " + peer[0] + ":" + str(peer[1]))

            env = generate_env(peer)
            print(env)

            p = subprocess.Popen(server_bin, shell=True, env=env, stdout=subprocess.PIPE)
            procs.append((peer, p))

        while True:
            for index, (peer, proc) in enumerate(procs):
                inline = proc.stdout.readline()

                if not inline:
                    break

                sys.stdout.write(colors[index] + "[" + ("PRIMARY" if peer[2] else "SECONDARY") + " PEER " + peer[0] + ":" + str(peer[1]) + "] ")
                sys.stdout.write(inline.decode() + reset)
                sys.stdout.flush()
    except KeyboardInterrupt:
        print("Captured keyboard interrupt")
        for (peer, proc) in procs:
            print("Killing process " + str(proc.pid))
            proc.send_signal(signal.SIGINT)



main()
