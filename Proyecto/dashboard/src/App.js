import React, { useState, useEffect } from 'react';
import axios from 'axios';
import MemoryPieChart from './components/MemoryPieChart';
import MemoryLineChart from './components/MemoryLineChart';
import PageFaultChart from './components/PageFaultChart';
import ActiveInactivePagesPie from './components/ActiveInactivePagesPie';
import TopProcessesTable from './components/TopProcessesTable';
import './App.css';

function App() {
  const [memoryData, setMemoryData] = useState(null);

  useEffect(() => {
    const fetchMemoryData = async () => {
      try {
        const response = await axios.get('http://127.0.0.1:5000/api/memory');
        setMemoryData(response.data);
      } catch (error) {
        console.error("Error:", error);
      }
    };

    fetchMemoryData();
  }, []);

  if (!memoryData) {
    return <div className="text-center mt-5">Cargando datos...</div>;
  }

  return (
    <div className="container mt-4">
      <h1 className="text-center mb-5">Monitoreo Avanzado De Memoria</h1>
      <div className="row">
        <div className="col-md-6">
          <div className="card mb-4">
            <div className="card-body">
              <h5 className="card-title text-center">Desglose Del Uso De La Memoria</h5>
              <MemoryPieChart memoryBreakdown={memoryData.memoryBreakdown} />
            </div>
          </div>
        </div>
        <div className="col-md-6">
          <div className="card mb-4">
            <div className="card-body">
              <h5 className="card-title text-center">Memory Usada A Lo Largo Del Tiempo</h5>
              <MemoryLineChart memoryUsageOverTime={memoryData.memoryUsageOverTime} />
            </div>
          </div>
        </div>
      </div>
      <div className="row">
        <div className="col-md-6">
          <div className="card mb-4">
            <div className="card-body">
              <h5 className="card-title text-center">Tasa De Fallos De Pagina</h5>
              <PageFaultChart pageFaults={memoryData.pageFaults} />
            </div>
          </div>
        </div>
        <div className="col-md-6">
          <div className="card mb-4">
            <div className="card-body">
              <h5 className="card-title text-center">Paginas Activas Vs Inactivas</h5>
              <ActiveInactivePagesPie activeInactivePages={memoryData.activeInactivePages} />
            </div>
          </div>
        </div>
      </div>
      <div className="row">
        <div className="col-md-12">
          <div className="card mb-4">
            <div className="card-body">
              <h5 className="card-title text-center">5 Procesos Que Consumen Memoria</h5>
              <TopProcessesTable topProcesses={memoryData.topProcesses} />
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;