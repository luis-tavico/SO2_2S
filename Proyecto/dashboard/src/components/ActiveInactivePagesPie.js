import React from 'react';
import { PieChart, Pie, Cell, Tooltip, Legend } from 'recharts';

const COLORS = ['#FF8042', '#0088FE'];

function ActiveInactivePagesPie({ activeInactivePages }) {
  
  const data = [
    { name: 'Paginas Activas',   value: activeInactivePages.active   },
    { name: 'Paginas Inactivas', value: activeInactivePages.inactive },
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

export default ActiveInactivePagesPie;