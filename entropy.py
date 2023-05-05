import math
import os

def shannon_entropy(data):
    freq_dict = {}
    for symbol in data:
        if symbol in freq_dict:
            freq_dict[symbol] += 1
        else:
            freq_dict[symbol] = 1
    
    entropy = 0.0
    total_symbols = len(data)
    for freq in freq_dict.values():
        probability = freq / total_symbols
        entropy += probability * math.log2(probability)
    
    return -entropy
sum = 0
count = 0
data = []
#Projdi všechny soubory ve složce data
for filename in os.listdir('data'):
    with open(os.path.join('data', filename), 'rb') as f:
        raw_data = f.read()
    data.append([int(b) for b in raw_data])
    entropy = shannon_entropy(data[-1])
    sum = sum + entropy
    count = count +1
    print(f" {filename} -> {entropy}")

print("Průměrná hodnota", sum/count)