import NeulandHit_pb2 as ProtoHits
import sys


def printHits(hits):
    for hit in hits.hit:
        print("hit position is %f, %f, %f" % (hit.position.x, hit.position.y, hit.position.z))


hits = ProtoHits.NeulandHits()

f = open(sys.argv[1], "rb")

hits.ParseFromString(f.read())
f.close()

printHits(hits)
