import React from 'react';
import { BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';

function PageFaultChart({ pageFaults }) {

  const pageFaultsData = [
    { name: 'Fallos Menores', count: pageFaults.minors },
    { name: 'Fallos Mayores', count: pageFaults.majors },
  ];

  return (
    <ResponsiveContainer width="100%" height={300}>
      <BarChart data={pageFaultsData}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="name" />
        <YAxis />
        <Tooltip />
        <Legend />
        <Bar 
          dataKey="count"
          name="Cantidad"
          fill="#8884d8" />
      </BarChart>
    </ResponsiveContainer>
  );
}

export default PageFaultChart;