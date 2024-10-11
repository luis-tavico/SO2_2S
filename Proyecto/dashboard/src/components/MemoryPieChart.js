import React from 'react';
import { PieChart, Pie, Cell, Tooltip, Legend } from 'recharts';

const COLORS = ['#0088FE', '#00C49F', '#FFBB28'];

function MemoryPieChart({ memoryInfo }) {
  
  const data = [
    { name: 'Memoria Libre', value: memoryInfo.free   },
    { name: 'Memoria Usada', value: memoryInfo.used   },
    { name: 'Memoria cache', value: memoryInfo.cached },
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