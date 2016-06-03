import skirl
import math

ar = skirl.readArray("rli76_angles_fine.dmp")
if(ar):
    print("File contains " + str(len(ar)) + " lines, "
          + str(len(ar[0])) + " samples each")
else:
    print("Unable to read array")
    exit(0)

for a in range(len(ar)):
    for b in range(len(ar[a])):
        re = ar[a][b][0]
        im = ar[a][b][1]
        amp = math.sqrt(re*re + im*im)
        if amp<50:
            ar[a][b][0] = 0 # Re
            ar[a][b][1] = 0 # Im

skirl.writeArray(ar, "rli76_angles_fine_1.dmp")
