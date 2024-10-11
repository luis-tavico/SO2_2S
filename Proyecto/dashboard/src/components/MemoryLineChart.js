import React from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';

function MemoryLineChart({ swapInfo }) {

  const data = [
    { name: 'Swap Libre', value: swapInfo.free },
    { name: 'Swap Usado', value: swapInfo.used },
  ];

  return (
    <ResponsiveContainer width="100%" height={300}>
      <LineChart data={data}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="name" />
        <YAxis />
        <Tooltip />
        <Legend />
        <Line 
          type="monotone"
          dataKey="value" 
          name="Cantidad"
          stroke="#8884d8" />
      </LineChart>
    </ResponsiveContainer>
  );
}

export default MemoryLineChart;