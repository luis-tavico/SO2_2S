import React from 'react';
import { PieChart, Pie, Cell, Tooltip, Legend } from 'recharts';

const COLORS = ['#0088FE', '#00C49F', '#FFBB28'];

function MemoryPieChart({ memoryBreakdown }) {
  const data = [
    { name: 'Used Memory', value: memoryBreakdown.used },
    { name: 'Free Memory', value: memoryBreakdown.free },
    { name: 'Cached Memory', value: memoryBreakdown.cached },
  ];

  return (
    <PieChart width={450} height={300}>
      <Pie
        data={data}
        cx={245}
        cy={130}
        labelLine={false}
        outerRadius={130}
        fill="#8884d8"
        dataKey="value"
      >
        {data.map((entry, index) => (
          <Cell key={`cell-${index}`} fill={COLORS[index % COLORS.length]} />
        ))}
      </Pie>
      <Tooltip />
      <Legend />
    </PieChart>
  );
}

export default MemoryPieChart;