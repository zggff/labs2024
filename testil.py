# test input leaks
import subprocess
import sys

pos = sys.argv.index("--") if "--" in sys.argv else len(sys.argv)
flags = [arg for arg in sys.argv[1:pos] if arg.startswith("-")]
args = [arg for arg in sys.argv[1:pos] if not arg.startswith("-")]
prog = sys.argv[pos + 1 :]

test_leaks = ["leaks", "--atExit", "--list", "--"]
prog = test_leaks + prog if "-t" in flags else prog

if len(args) == 1:
    with open(args[0]) as f:
        inp = f.read()
        p = subprocess.run(
            prog,
            text=True,
            input=inp,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )
        print(f"\n{p.stdout}")
elif len(args) > 1:
    with open(args[0]) as f_in, open(args[1]) as f_out:
        inp = f_in.read()
        p = subprocess.run(
            prog,
            text=True,
            input=inp,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )
        f_out.write(f"\n{p.stdout}")
else:
    try:
        p = subprocess.run(
            prog,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )
        print(f"\n{p.stdout.decode()}")
    except KeyboardInterrupt:
        pass
