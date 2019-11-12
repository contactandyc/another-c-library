import React from "react"
import { Link } from "gatsby"

/* for dynamically rendering h1s
function Subheadings(props) {
  if (props.active) {
    return (
      <ul style={props.styles.ul}>
        {props.i.headings.map(({ value: x }) => {
          let id = x.replace(/\s+/g, '-').toLowerCase();

          return (
            <li key={id} style={props.styles.li}>
              <Link
              to={`/${props.type}${props.i.frontmatter.path}#${id}`}
              activeStyle={props.styles.activeLink}>
                {x}
              </Link>
            </li>
          )
        })}
      </ul>
    )
  }
  else return null
}
*/

function Sidebar(props) {
  //const [active, setActive] = useState(null)
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

  /* for dynamically rendering h1s
  const isPartiallyActive = ({ isPartiallyCurrent }) => {
    console.log("isPartiallyCurrent", isPartiallyCurrent)
    if (isPartiallyCurrent)
      setActive(true)
    else
      setActive(null)
  }*/

  console.log(props.allPages)

  return (
    <div style={styles.sidebar} className="Sidebar">
      <div>
        {props.allPages.edges
          .filter(i => i.node.frontmatter.title.length > 0)
          .map(({ node: i }) => {
            return (
              <div key={i.id}>
                <Link
                  to={`/${props.type}${i.frontmatter.path}`}
                  activeStyle={styles.activeLink}
                  >
                    {i.frontmatter.title}
                </Link>

                {/* getProps={isPartiallyActive} console.log("active", active)}

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
                </ul>*/}

              </div>
            )
          })
        }
      </div>
    </div>
  );
}

export default Sidebar
