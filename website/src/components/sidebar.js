import React, { useState } from "react";
import { Link } from "gatsby";
import "../pages/index.css";

function Headings(props) {
  if (props.title === props.curr) {
    return (
      <ul className="Sidebar-ul" style={{ display: props.expand }}>
        {props.arr.map(({ value: val }) => {
          let id = val.replace(/\s+/g, '-').toLowerCase();
          return (
            <li key={id} className="Sidebar-li">
              <Link to={`/${props.type}${props.path}#${id}`}>
                {val}
              </Link>
            </li>
          );
        })}
      </ul>
    );
  } else {
      return <></>;
  }
}

function Sidebar(props) {
  const [expand, setExpand] = useState("");
  const updateExpand = () => setExpand("none");

  let arr = props.allPages.edges.filter(i => i.node.frontmatter.title.length > 0);
  arr = arr.filter(i => i.node.frontmatter.posttype === props.type);

  if (props.type === "tutorial") { // sort tutorial pages numerically
    arr.sort(function(a, b) {
      let ai = parseInt(a.node.frontmatter.title);
      let bi = parseInt(b.node.frontmatter.title);
      return ai - bi;
    });
  }

  return (
    <div className="Sidebar">
      <div>
        {arr.map((val, i) => {
          return (
            <div key={i}>
              <Link
                to={`/${props.type}${val.node.frontmatter.path}`}
                activeClassName="Sidebar-actv"
                onClick={updateExpand}
                >
                  {val.node.frontmatter.title}
              </Link>
              <Headings
                arr={val.node.headings}
                curr={props.current}
                title={val.node.frontmatter.title}
                type={props.type}
                path={val.node.frontmatter.path}
                expand={expand} />
            </div>
          );
        })}
      </div>
    </div>
  );
}

export default Sidebar;
