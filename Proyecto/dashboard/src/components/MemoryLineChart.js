import React from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';

function MemoryLineChart({ memoryUsageOverTime }) {
  return (
    <ResponsiveContainer width="100%" height={300}>
      <LineChart data={memoryUsageOverTime}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="time" />
        <YAxis />
        <Tooltip />
        <Legend />
        <Line type="monotone" dataKey="physicalMemory" stroke="#8884d8" activeDot={{ r: 8 }} />
        <Line type="monotone" dataKey="swapMemory" stroke="#82ca9d" />
      </LineChart>
    </ResponsiveContainer>
  );
}

export default MemoryLineChart;