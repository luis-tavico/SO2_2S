import React from 'react';
import { BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';

function PageFaultChart({ pageFaults }) {
  return (
    <ResponsiveContainer width="100%" height={300}>
      <BarChart data={pageFaults}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="time" />
        <YAxis />
        <Tooltip />
        <Legend />
        <Bar dataKey="minorFaults" fill="#8884d8" />
        <Bar dataKey="majorFaults" fill="#82ca9d" />
      </BarChart>
    </ResponsiveContainer>
  );
}

export default PageFaultChart;