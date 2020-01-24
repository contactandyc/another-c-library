/*
  Building a drag and drop card game for data structures
  Purely experimental at this point, do not link to this page.

*/

import React, { useState } from 'react';
//import ReactDOM from 'react-dom';
import { DragDropContext, Droppable, Draggable } from 'react-beautiful-dnd';

/*
  1. choose a pivot
    -  click to set pivot
  2. move items greater than pivot to the right
    - click and drag to arrange items one side at a time
      -- choose active partition, L or R of pivot
      -- if draggable < pivot, color = red && do nothing
      -- if draggable.side != active partition, color = red && do nothing
  3. recursively apply 1. & 2. on partitions

*/

// array generator
const getItems = count => {
  let arr = [];
  for(let i=0; i<count; i++)
    arr.push({ id: `item-${i}`, content: `Item ${i}` });

  // randomize array
  let curr = arr.length, temp, rand;

  while (0 !== curr) {
    rand = Math.floor(Math.random() * curr);
    curr -= 1;

    // And swap it with the current element.
    temp = arr[curr];
    arr[curr] = arr[rand];
    arr[rand] = temp;
  }

  return arr;
}

// styles
const getItemStyle = (isDragging, draggableStyle) => ({
  userSelect: 'none',
  padding: 16,
  margin: `0 ${8}px 0 0`,
  background: isDragging ? 'lightgreen' : 'grey',
  // styles we need to apply on draggables
  ...draggableStyle,
});

const getListStyle = isDraggingOver => ({
  background: isDraggingOver ? 'lightblue' : 'lightgrey',
  display: 'flex',
  padding: 8,
  overflow: 'auto',
});

function Card(props) {
  return (
    <div
      ref={props.pv.innerRef}
      {...props.pv.draggableProps}
      {...props.pv.dragHandleProps}
      style={getItemStyle(props.ss.isDragging, props.pv.draggableProps.style)}
      onClick={(e) => props.updatePivot}>
      {props.item.content}
    </div>
  );
}

function CardList() {
  const [items, setItems] = useState(getItems(10));
  const [pivot, setPivot] = useState();
  const [left, setLeft] = useState();
  const [right, setRight] = useState();
  const [list1, setList1] = useState({});
  const [list2a, setList2a] = useState({});
  const [list2b, setList2b] = useState({});
  const [list3a, setList3a] = useState({});
  const [list3b, setList3b] = useState({});

  // update pivot card
  const updatePivot = e => {
    if (e.target.value)
      setList1({ color: "green" });
    else
      setList1({ color: "black" });

    setPivot(e.target.value);

    setList2a({ color: "green" });
    let left = items.slice(0, e.target.value);
    if (!left.filter(i => i > e.target.value))
      return;

    setList2a({ color: "black" });
    setList2b({ color: "green" });
    let right = items.slice(e.target.value, items.length);
    if (!right.filter(i => i < pivot))
      return;
  }

  // reorder array of cards (DND func)
  const reorder = (list, start, end) => {
    const arr = list;
    const [rm] = arr.splice(start, 1);
    arr.splice(end, 0, rm);
    return arr;
  };

  // action after drop (DND func)
  const onDragEnd = result => {
    if (!result.destination)
      return;
    setItems(reorder(items, result.source.index, result.destination.index));
  }

  return (
    <>
      <DragDropContext onDragEnd={onDragEnd}>
        <Droppable droppableId="droppable" direction="horizontal">
          {(provided, snapshot) => (
            <div
              ref={provided.innerRef}
              style={getListStyle(snapshot.isDraggingOver)}
              {...provided.droppableProps}>

              {items.map((item, index) => (
                <Draggable key={item.id} draggableId={item.id} index={index}>
                  {(provided, snapshot) => (
                    <Card item={item} pv={provided} ss={snapshot} />
                  )}
                </Draggable>
              ))}

              {provided.placeholder}
            </div>
          )}
        </Droppable>
      </DragDropContext>

      {/*
        Highlight active step. Apply rules based on steps.
        1.  starts: when input box === ""
            function: update pivot number
                      pivot cannot be moved
            finishes: when input has number

        2a. starts: when input box has number
            function: slices array at pivot, takes left side
                      checks for elements larger than pivot in new array
            finishes: when elements larger than pivot don't exist

        2b. starts: when 2a finishes
            function: slices array at pivot, takes right side
                      checks for elements lesser than pivot in new array
            finishes: when elements smaller than pivot don't exist

        3a. starts: when 2b finishes
            function: return right and left of pivot
                      choose either right or left
                      repeat 1, 2a, 2b
      */}

      <div style={{ margin: "20px" }}>
        <input placeholder="Pivot" value={pivot} onChange={(e) => updatePivot(e)}></input><br />
        <h1>Hoare's Quicksort</h1>
        <ul style={{ listStyle:"none" }}>
          <li style={list1}>1. Choose a pivot{console.log(list1)}</li>
          <li style={list2a}>2a. For cards left of pivot: if cards are greater than pivot, move to right of pivot</li>
          <li style={list2b}>2b. For cards right of pivot: if cards are less than pivot, move to left of pivot</li>
          <li style={list3a}>3a. Choose an active partition (left or right of pivot)</li>
          <li style={list3b}>3b. Recursively apply steps 1 & 2 on chosen partition</li>
        </ul>
      </div>
    </>
  );
}

export default CardList;
