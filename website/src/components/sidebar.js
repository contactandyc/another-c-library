import React, { useState } from "react";
import { Link } from "gatsby";
import { MdKeyboardArrowRight, MdKeyboardArrowDown } from "react-icons/md";
import "../pages/index.css";

function Headings(props) {
  return (
    <ul className="Sidebar-ul">
      {props.cld.map(({ value: val }) => {
        let id = val.replace(/\s+/g, '-').toLowerCase();
        return (
          <li key={id} style={{ marginBottom: 0 }}>
            <Link style={{ color: "black" }} to={`/${props.pth}#${id}`}>
              {val}
            </Link>
          </li>
        );
      })}
    </ul>
  );
}

function MenuItem(props) {
  const [toggle, setToggle] = useState(false);
  const updateToggle = () => setToggle(!toggle);

  if (toggle) {
    return (
      <div>
        <div className="Sidebar-btn">
        <Link
          activeStyle={{ color: "#003BFF", fontWeight: 600 }}
          style={{ color: "black" }}
          to={props.pth}
          >
          {props.ttl}
        </Link>
        <MdKeyboardArrowDown
          onClick={updateToggle}
          role="button"
          style={{ cursor: "pointer", paddingTop: 2 }} />
        </div>
        <Headings
          cld={props.cld}
          pth={props.pth}
          ttl={props.ttl}
          typ={props.typ}
        />
      </div>
    );
  }
  else {
    return (
      <div className="Sidebar-btn">
        <Link
          activeStyle={{ color: "#003BFF", fontWeight: 600 }}
          style={{ color: "black" }}
          partiallyActive={true}
          to={props.pth}
          >
          {props.ttl}
        </Link>
        <MdKeyboardArrowRight
          onClick={updateToggle}
          role="button"
          style={{ cursor: "pointer", paddingTop: 2 }} />
      </div>
    );
  }
}

function Sidebar({ pages, type }) {
  let arr = pages.edges.filter(i => i.node.frontmatter.title.length > 0);
  arr = arr.filter(i => i.node.frontmatter.posttype === type);

  // sort tutorial pages numerically
  if (type === "tutorial") {
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
            <MenuItem
              key={i}
              pth={`/${type}${val.node.frontmatter.path}`}
              ttl={val.node.frontmatter.title}
              cld={val.node.headings}
              typ={type}
            />
          );
        })}
      </div>
    </div>
  );
}

export default Sidebar;
