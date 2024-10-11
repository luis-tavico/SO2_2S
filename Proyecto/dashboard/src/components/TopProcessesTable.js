import React from 'react';

function TopProcessesTable({ topProcesses }) {
  return (
    <table className="table table-hover table-Light text-center">
      <thead>
        <tr>
          <th>PID</th>
          <th>Nombre de Proceso</th>
          <th>Memoria Usada (%)</th>
        </tr>
      </thead>
      <tbody>
        {topProcesses.map((process) => (
          <tr key={process.pid}>
            <td>{process.pid}</td>
            <td>{process.name}</td>
            <td>{process.memoryUsage}</td>
          </tr>
        ))}
      </tbody>
    </table>
  );
}

export default TopProcessesTable;