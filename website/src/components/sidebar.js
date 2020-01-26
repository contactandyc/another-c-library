import React, { useState } from "react"
import { Link } from "gatsby"

function Headings(props) {
  if (props.title === props.curr) {
    return (
      <ul style={{ paddingLeft: 0, listStyle: `none`, display: props.expand }}>
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
  } else {
    return (
      <></>
    );
  }
}

function Sidebar(props) {
  const [expand, setExpand] = useState("");
  const updateExpand = () => setExpand("none");

  let styles = {
    sidebar: {
      fontFamily: `-apple-system,'BlinkMacSystemFont','Segoe UI','Roboto','Oxygen',
      'Ubuntu','Cantarell','Fira Sans','Droid Sans','Helvetica Neue',sans-serif`,
      height: `100%`,
      padding: `15px`,
      color: `#1E4E8C`,
      position: `sticky`,
      top: `62px`,
      borderLeft: `solid 1px #EDF2F7`,
      overflowY: `scroll`
    },
    activeLink: {
      color: `#1A202C`,
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

  props.allPages.edges.sort(function(a, b) {
    let ai = parseInt(a.node.frontmatter.title);
    let bi = parseInt(b.node.frontmatter.title);
    return ai - bi;
  });

  let elmArr = props.allPages.edges.filter(elm => elm.node.frontmatter.title.length > 0);

  elmArr = elmArr.filter(elm => elm.node.frontmatter.posttype === props.type);

  return (
    <div style={styles.sidebar} className="Sidebar">
      <div>
        {elmArr.map((val, i) => {
          return (
              <div key={i}>
                <Link
                  to={`/${props.type}${val.node.frontmatter.path}`}
                  activeStyle={styles.activeLink}
                  onClick={updateExpand}>
                    {val.node.frontmatter.title}
                </Link>
                <Headings
                  arr={val.node.headings}
                  curr={props.current}
                  title={val.node.frontmatter.title}
                  style={styles}
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
