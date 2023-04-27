import sys

if len(sys.argv) != 3:
    print("Usage: python compare_raw.py file1.raw file2.raw")
    sys.exit(1)

file1 = sys.argv[1]
file2 = sys.argv[2]

with open(file1, 'rb') as f1, open(file2, 'rb') as f2:
    data1 = f1.read()
    data2 = f2.read()

if len(data1) != len(data2):
    print("Files have different lengths")
    print(len(data1))
    print(len(data2))
    sys.exit(1)

diff_count = 0
for i in range(len(data1)):
    if data1[i] != data2[i]:
        print(f"Difference at byte {i}")
        diff_count += 1

if diff_count == 0:
    print("Files are identical")
else:
    print(f"Total differences: {diff_count}")