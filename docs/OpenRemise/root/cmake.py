import esptool
import sys
import os
import json
from datetime import datetime

def run():
    # Nur das Verzeichnis als Parameter, damit das Script weiß, wo es arbeiten soll
    dest_dir = sys.argv[1]
    fw_dir = sys.argv[2]
    version = sys.argv[3]
    postfix = sys.argv[4].strip()
    json_version = f"v{version} {postfix}".strip()
    os.chdir(fw_dir)
    hwKey = "openremise"
    current_date = datetime.now().strftime("%Y-%m-%d")

    # Der Befehl - exakt deine Dateien, exakt deine Offsets
    args = [
        '--chip', 'esp32s3',
        'merge_bin',
        '-o', 'Firmware_Full.bin',
        '--flash_mode', 'dout',
        '--flash_freq', '80m',
        '--flash_size', '32MB',
        '0x0', 'bootloader/bootloader.bin',
        '0x8000', 'partition_table/partition-table.bin',
        '0x9000', 'ota_data_initial.bin',
        '0x10000', 'Firmware.bin',
        '0xc10000', 'nvs.bin'
    ]

    try:
        esptool.main(args)
    except Exception as e:
        print(f"Fehler: {e}")
        sys.exit(1)

    json_path = os.path.join(dest_dir, "versions.json")
    if os.path.exists(json_path):
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        updated = False
        # Definition der IDs für den Check
        full_id = f"v{version}{postfix}/Firmware_Full.bin"
        app_id = f"v{version}{postfix}/Firmware.bin"

        # Check für FULL
        if os.path.exists(os.path.join(fw_dir, "Firmware_Full.bin")):
            existing_item = next((item for item in data[hwKey]["full"] if item['id'] == full_id), None)
            if existing_item:
                # Eintrag aktualisieren
                existing_item["name"] = f"{current_date} {json_version}"
                updated = True
            else:
                data[hwKey]["full"].insert(0, {"id": full_id, "name": f"{current_date} {json_version}", "offset": "0x0"})
                updated = True

        # Check für UPDATE (App)
        if os.path.exists(os.path.join(fw_dir, "Firmware.bin")):
            existing_item = next((item for item in data[hwKey]["update"] if item['id'] == app_id), None)
            if existing_item:
                # Eintrag aktualisieren
                existing_item["name"] = f"{current_date} {json_version}"
                updated = True
            else:
                data[hwKey]["update"].insert(0, {"id": app_id, "name": f"{current_date} {json_version}", "offset": "0x10000"})
                updated = True

        if updated:
            with open(json_path, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=2)
            print("JSON erfolgreich aktualisiert.")

if __name__ == '__main__':
    run()