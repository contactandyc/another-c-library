import React, { useState, useEffect } from "react"
import { Link } from "gatsby"

function Headings(props) {
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

function Sidebar(props) {
  const styles = {
    sidebar: {
      fontFamily: `-apple-system,'BlinkMacSystemFont','Segoe UI','Roboto','Oxygen',
      'Ubuntu','Cantarell','Fira Sans','Droid Sans','Helvetica Neue',sans-serif`,
      height: `100vh`,
      padding: `15px`,
      color: `#1E4E8C`,
      position: `sticky`,
      top: `62px`,
      borderLeft: `solid 1px #EDF2F7`
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

  const [active, setActive] = useState();

  const elmArr = props.allPages.edges.filter(elm => elm.node.frontmatter.title.length > 0);

  let headings;

  useEffect(() => {
    if (active) {
      headings = <Headings
                    arr={active.node.headings}
                    style={styles}
                    path={active.node.frontmatter.path}
                    type={props.type} />
    }
  });

  return (
    <div style={styles.sidebar} className="Sidebar">
      <div>
        {elmArr.map((val, i) => {
          return (
              <div key={i}>
                <Link
                  to={`/${props.type}${val.node.frontmatter.path}`}
                  activeStyle={styles.activeLink}
                  onClick={() => setActive(val)}>
                    {val.node.frontmatter.title}
                </Link>
                {headings}
              </div>
          );
        })}
      </div>
    </div>
  );
}

export default Sidebar

/*
<Subheadings active={active} i={i} type={props.type} styles={styles} />

<ul style={styles.ul}>
  {i.headings.map(({ value: x }) => {
    let id = x.replace(/\s+/g, '-').toLowerCase();

    return (
      <li key={id} style={styles.li}>
        <Link
        to={`/${props.type}${i.frontmatter.path}#${id}`}
        activeStyle={styles.activeLink}>
          {x}
        </Link>
      </li>
    )
  })}
</ul>}
*/
