from flask import Flask, jsonify
from flask_cors import CORS
import random
import time

app = Flask(__name__)
CORS(app)

# Simular datos de uso de memoria
def get_fake_memory_data():
    '''
    return {
    "mem_used": 2048000,
    "mem_free": 1024000,
    "mem_cached": 512000,
    "swap_used": 256000,
    "swap_free": 256000,
    "minor_faults": 1234,
    "major_faults": 12,
    "active_pages": 10000,
    "inactive_pages": 5000,
    "top_processes": [
        {"pid": 1234, "name": "proc1", "mem_usage": 1048576},
        {"pid": 2345, "name": "proc2", "mem_usage": 524288},
        {"pid": 3456, "name": "proc3", "mem_usage": 262144},
        {"pid": 4567, "name": "proc4", "mem_usage": 131072},
        {"pid": 5678, "name": "proc5", "mem_usage": 65536}
    ]
}

    '''
    return {
        "memoryBreakdown": {
            "used": random.randint(2000, 8000),
            "free": random.randint(1000, 3000),
            "cached": random.randint(500, 1500),
        },
        "memoryUsageOverTime": [
            {"time": time.time() - 600, "physicalMemory": random.randint(2000, 8000), "swapMemory": random.randint(0, 2000)},
            {"time": time.time() - 300, "physicalMemory": random.randint(2000, 8000), "swapMemory": random.randint(0, 2000)},
            {"time": time.time(), "physicalMemory": random.randint(2000, 8000), "swapMemory": random.randint(0, 2000)},
        ],
        "pageFaults": [
            {"time": time.time() - 600, "minorFaults": random.randint(0, 100), "majorFaults": random.randint(0, 50)},
            {"time": time.time() - 300, "minorFaults": random.randint(0, 100), "majorFaults": random.randint(0, 50)},
            {"time": time.time(), "minorFaults": random.randint(0, 100), "majorFaults": random.randint(0, 50)},
        ],
        "activeInactivePages": {
            "active": random.randint(1000, 5000),
            "inactive": random.randint(1000, 5000),
        },
        "topProcesses": [
            {"name": "Process 1", "pid": 1234, "memoryUsage": random.uniform(5, 30)},
            {"name": "Process 2", "pid": 2345, "memoryUsage": random.uniform(5, 30)},
            {"name": "Process 3", "pid": 3456, "memoryUsage": random.uniform(5, 30)},
            {"name": "Process 4", "pid": 4567, "memoryUsage": random.uniform(5, 30)},
            {"name": "Process 5", "pid": 5678, "memoryUsage": random.uniform(5, 30)},
        ]
    }

# Ruta para obtener datos simulados
@app.route('/api/memory', methods=['GET'])
def memory_data():
    return jsonify(get_fake_memory_data())

if __name__ == '__main__':
    app.run(debug=True)