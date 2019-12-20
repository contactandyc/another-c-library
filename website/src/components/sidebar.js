import React from "react"
import { Link } from "gatsby"

function Sidebar(props) {
  const styles = {
    sidebar: {
      fontFamily: `-apple-system,'BlinkMacSystemFont','Segoe UI','Roboto','Oxygen','Ubuntu','Cantarell','Fira Sans','Droid Sans','Helvetica Neue',sans-serif`,
      width: `400px`,
      height: `100vh`,
      padding: `15px`,
      color: `#1E4E8C`,
      position: `sticky`,
      top: `62px`,
      borderLeft: `solid 1px #EDF2F7`
    },
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

  const collator = new Intl.Collator(undefined, {numeric: true, sensitivity: 'base'});
  let titles = [];
  let paths = [];

  for (let i=0; i<props.allPages.edges.length; i++) {
    if (props.allPages.edges[i].node.frontmatter.title !== "") {
      titles.push(props.allPages.edges[i].node.frontmatter.title);
      paths.push(props.allPages.edges[i].node.frontmatter.path);
    }
  }

  titles.sort(collator.compare);
  paths.sort(collator.compare);

  return (
    <div style={styles.sidebar} className="Sidebar">
      <div>
        {titles.map((val, i) => {
          return (
            <div key={i}>
              <Link
                to={`/${props.type}${paths[i]}`}
                activeStyle={styles.activeLink}>
                  {val}
              </Link>
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
