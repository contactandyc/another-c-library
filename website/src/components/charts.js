import React, { useState, useEffect, useRef } from "react";
import {
  VictoryChart,
  VictoryBar,
  VictoryAxis,
  VictoryLabel,
  VictoryPie } from "victory";

function useInterval(callback, delay) {
  const savedCallback = useRef();

  // Remember the latest callback.
  useEffect(() => {
    savedCallback.current = callback;
  }, [callback]);

  // Set up the interval.
  useEffect(() => {
    function tick() {
      savedCallback.current();
    }
    if (delay !== null) {
      let id = setInterval(tick, delay);
      return () => clearInterval(id);
    }
  }, [delay]);
}

export function BarChart() {
  const dataArray = [
    [
      {x: 1, y: 393.470},
      {x: 2, y: 3315.230},
      {x: 3, y: 4379.690}
    ],
    [
      {x: 1, y: 384.580},
      {x: 2, y: 3202.080},
      {x: 3, y: 2784.150}
    ],
    [
      {x: 1, y: 963.120},
      {x: 2, y: 5984.830},
      {x: 3, y: 22208.090}
    ],
    [
      {x: 1, y: 60816.670},
      {x: 2, y: 77113.870},
      {x: 3, y: 111768.980}
    ]
  ];
  const nameArr = ["Ordered", "Equal", "Reversed", "Random"];
  const [count, setCount] = useState(0);
  const [data, setData] = useState(dataArray[count]);
  const [name, setName] = useState(nameArr[count]);

  useInterval(() => {
    if (count < 3)
      setCount(count + 1);
    else
      setCount(0);

    setName(nameArr[count]);
    setData(dataArray[count]);
  }, 3000);

  return (
    <VictoryChart
      domainPadding={{ x: 50 }}
      animate={{duration: 1000}}
    >
      <VictoryAxis
        tickValues={[1, 2, 3]}
        tickFormat={["ac_sort", "std::sort", "qsort"]}
        label={`1 Million Records: ${name}`}
        style={{ fontFamily: "system-ui" }}
        axisLabelComponent={<VictoryLabel dy={5} style={{ fontWeight: "bold", fontSize: "16", fontFamily: "system-ui"}}/>}
      />
      <VictoryAxis
        dependentAxis
        label="Time (microseconds)"
        tickFormat={(x) => (`${x}`)}
        axisLabelComponent={<VictoryLabel dy={45} />}
        tickLabelComponent={<VictoryLabel dx={5}/>}
      />
      <VictoryBar
        data={data}
        style={{
          data: {
            fill: ({ datum }) => datum.x === 1 ? "#0DB14B" : "tomato",
            width: 30
          }
        }}
      />
    </VictoryChart>
  );
}

export function PieChart() {
  const dataset = [
    [
      { x: 1, y: 187, label: "ac_json: 187 (Mbps)" },
      { x: 2, y: 1192, label: "rapidjson: 1192 (Mbps)" },
    ],
    [
      { x: 1, y: 374, label: "ac_json: 374 (Mbps)" },
      { x: 2, y: 1223, label: "rapidjson: 1222 (Mbps)" },
    ]
  ];

  return (
    <VictoryPie
      colorScale={["#0DB14B", "#8AA7B5" ]}
      data={dataset[0]}
      labelRadius={60}
      style={{ labels: { color: "white", fontWeight: "bold", fontSize: "16", fontFamily: "system-ui" } }}
    />
  );
}
