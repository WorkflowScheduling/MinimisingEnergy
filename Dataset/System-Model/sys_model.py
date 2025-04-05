import random
import csv
from pprint import pprint


vm_types = ["small", "medium", "large", "xlarge", "xxlarge"]
power_ranges_by_vm_size = {
    "small": (0.1, 0.7),       
    "medium": (0.6, 10),        
    "large": (11, 50),        
    "xlarge": (43, 75),    
    "xxlarge": (100, 200)    
}

frequency_ranges = (0.12, 5.5) # In Ghz
ro_vals = (1e-5, 1e-6, 1e-4)

# Minimax normalisation
def normalize(value, min_value, max_value):
    return (value - min_value) / (max_value - min_value)


def generate_cloud_data(cloud_providers):
    all_cloud_data = []
    for provider in cloud_providers:
        cloud_vms = []
        
        vm_freq_ranges = [(frequency_ranges[0] + i * (frequency_ranges[1] - frequency_ranges[0]) / len(vm_types),
                           frequency_ranges[0] + (i + 1) * (frequency_ranges[1] - frequency_ranges[0]) / len(vm_types))
                          for i in range(len(vm_types))]
        
        for vm_type, freq_range in zip(vm_types, vm_freq_ranges):

            power_range = power_ranges_by_vm_size[vm_type]
            
            num_frequencies = random.randint(4, 6)
            frequencies = sorted([round(random.uniform(*freq_range), 2) for _ in range(num_frequencies)])
            normalized_frequencies = [round(normalize(f, frequencies[0]-1e-2, frequencies[-1]), 2) for f in frequencies]
            
            p_min = round(random.uniform(*power_range), 2)
            p_max = round(random.uniform(1.5 * p_min, 3 * p_min), 2)
            ro_val = ro_vals[random.randint(0,2)]
            
            cloud_vms.append({
                "cloud_provider": provider,
                "vm_type": vm_type,
                "num_freq":num_frequencies,
                "frequencies": normalized_frequencies,
                "p_static": p_min,
                "p_dynamic": p_max,
                "ro_val": ro_val,
                "w_val" : random.randint(3,7)
            })
        
        all_cloud_data.append(cloud_vms)

    return all_cloud_data

cloud_providers = ["Cloud_1", "Cloud_2", "Cloud_3"]
cloud_data = generate_cloud_data(cloud_providers)

with open('system_model_data.csv', mode='w', newline='') as csv_file:
    fieldnames = ["cloud_provider", "vm_type", "num_freq",
                  "frequencies", "p_static", "p_dynamic", "ro_val", "w_val"]
    
    writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
    writer.writeheader()
    
    for cloud in cloud_data:
        for vm in cloud:
            writer.writerow({
                fieldnames[0] : vm[fieldnames[0]],
                fieldnames[1] : vm[fieldnames[1]],
                fieldnames[2] : vm[fieldnames[2]],
                fieldnames[3]: str(' '.join([str(x) for x in vm[fieldnames[3]]])),
                fieldnames[4]: vm[fieldnames[4]],
                fieldnames[5]: vm[fieldnames[5]],
                fieldnames[6]:vm[fieldnames[6]],
                fieldnames[7]:vm[fieldnames[7]]
            })

print("Data written to CSV")
