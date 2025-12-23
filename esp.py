import serial
import time
import json
import os
import msvcrt 

PORT = "COM5"
BAUD = 115200
SAVE_DIR = "esp_readings"

TOTAL_READINGS = 7
WINDOW_TIME = 10   # seconds

os.makedirs(SAVE_DIR, exist_ok=True)

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

print("Serial Connected")
print("LIVE MODE STARTED...")
print("Press 'c' to START 7×10 second batch capture\n")

def average_samples(samples):
    avg = {}
    keys = samples[0].keys()
    for k in keys:
        avg[k] = sum(s[k] for s in samples) / len(samples)
    return avg

# ---------------- LIVE MODE ----------------
while True:
    try:
        if ser.in_waiting:
            line = ser.readline().decode(errors="ignore").strip()

            if line.startswith("{") and line.endswith("}"):
                data = json.loads(line)
                print("LIVE →", data)

        # Check keyboard (NON-BLOCKING)
        if msvcrt.kbhit():
            key = msvcrt.getch().decode().lower()

            if key == "c":
                print("\n✅ CAPTURE MODE STARTED\n")
                break

        time.sleep(0.05)

    except Exception as e:
        print("❌ Live Error:", e)


# ---------------- BATCH CAPTURE MODE ----------------
reading_index = 1

while reading_index <= TOTAL_READINGS:
    print(f"⏳ Collecting Reading #{reading_index} for 10 seconds...")

    start = time.time()
    collected = []

    while time.time() - start < WINDOW_TIME:
        try:
            line = ser.readline().decode(errors="ignore").strip()

            if line.startswith("{") and line.endswith("}"):
                data = json.loads(line)
                collected.append(data)
                print("✔", data)

        except Exception as e:
            print("Bad Data:", e)
                     
    if not collected:
        print("⚠ No data collected, retrying...")
        continue
 
    avg = average_samples(collected)             

    final_json = {
        "UV": round(avg["UV"], 4),
        "TDS": round(avg["TDS"], 4),
        "pH": round(avg["pH"], 4),
        "Pt": round(avg["Pt"], 3),
        "Cu": round(avg["Cu"], 3),
        "Zn": round(avg["Zn"], 3),
        "SS": round(avg["SS"], 3),
        "Ag": round(avg["Ag"], 3),
        "Graphite": round(avg["Graphite"], 3),
        "Temp": round(avg["Temp"], 2),

    }

    filename = f"reading_{reading_index}.json"
    path = os.path.join(SAVE_DIR, filename)

    with open(path, "w") as f:
        json.dump(final_json, f, indent=4)

    print(f"✅ Saved {filename}\n")

    reading_index += 1

print("ALL SERIAL READINGS COMPLETE")
ser.close()








