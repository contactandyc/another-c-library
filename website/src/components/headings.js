import React, { useState, useEffect } from "react"
import { Link } from "gatsby"

function Headings(props) {
  const styles = {
    activeLink: {
      color: `#38A169`,
      fontWeight: `700`
    },
    ul: {
      paddingLeft: `0`,
      listStyle: `none`
    },
    li: {
      marginBottom: `0`
    }
  }

  return (
    <ul style={props.style.ul}>
      {props.arr.map(({ value: val }) => {
        let id = val.replace(/\s+/g, '-').toLowerCase();
        return (
          <li key={id} style={props.style.li}>
            <Link
              to={`/${props.type}${props.path}#${id}`}
              activeStyle={props.style.activeLink}>
              {val}
            </Link>
          </li>
        );
      })}
    </ul>
  );
}

export default Headings;
