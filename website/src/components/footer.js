import React from "react"
import { graphql, useStaticQuery, Link } from "gatsby"
import { FaExternalLinkAlt } from "react-icons/fa"

function Footer() {
  // posttype under path
  const data = useStaticQuery(graphql`
    query SitePages {
      allMarkdownRemark(sort: {order: ASC, fields: [frontmatter___title]}) {
        edges {
          node {
            id
            frontmatter {
              title
              path
            }
          }
        }
      }
    }
  `)
  const styles = {
    footer: {
      backgroundColor: `#1A202C`,
      padding: `10px`,
      color: `white`,
      fontFamily: `-apple-system,'BlinkMacSystemFont','Segoe UI','Roboto','Oxygen','Ubuntu','Cantarell','Fira Sans','Droid Sans','Helvetica Neue',sans-serif`
    },
    ul: {
      listStyle: `none`,
      marginLeft: 0,
    },
    link: {
      color: `#3182CE`,
    },
    p: {
      marginBottom: `15px`
    }
  }

  return (
    <footer style={styles.footer}>
      <div className="Footer">

        <div>
          <h1 style={{ marginBottom: `5px` }}>Another C Library</h1>
          <p style={styles.p}>© {new Date().getFullYear()} Andy Curtis & Daniel Curtis</p>
          <ul style={styles.ul}>
            <li><Link style={styles.link} to="/contact">Contact</Link></li>
            <li><Link style={styles.link} to="/contributing">Contributing</Link></li>
            <li><Link style={styles.link} to="/license">License</Link></li>
          </ul>
        </div>

        <div>
          <h2 style={{ margin: `10px auto` }}>A C eBook</h2>
          <ul style={styles.ul}>
            {data.allMarkdownRemark.edges
              /*.filter(i => i.node.frontmatter.posttype === "ebook")*/
              .map(({ node: i }) => {
                return (
                  <li key={i.id}>
                    <Link
                      to={`/ebook/${i.frontmatter.path}`}
                      activeStyle={styles.activeLink}>
                      {i.frontmatter.title}
                    </Link>
                  </li>
                )
              })
            }
          </ul>
        </div>

        <div>
          {/* commenting out until documentation is built
          <h2 style={{ margin: `10px auto` }}>Documentation</h2>
          <ul style={styles.ul}>
            {data.allMarkdownRemark.edges
              .filter(i => i.node.frontmatter.posttype === "doc")
              .map(({ node: i }) => {
                return (
                  <li key={i.id}>
                    <Link
                      to={`/docs/${i.frontmatter.path}`}
                      activeStyle={styles.activeLink}>
                      {i.frontmatter.title}
                    </Link>
                  </li>
                )
              })
            }
          </ul>*/}

          <h2 style={{ margin: `10px auto` }}>Resources</h2>
          <ul style={styles.ul}>
            <li>
              <a href="https://medium.com/software-design/why-software-developers-should-care-about-cpu-caches-8da04355bb8a" target="_blank" rel="noopener noreferrer" style={styles.link}>
                Why software developers should care about CPU caches <FaExternalLinkAlt />
              </a>
            </li>
            <li>
              <a href="https://github.com/contactandyc/another-c-library/blob/master/docs/tips.md#create-multiline-macro-in-c" target="_blank" rel="noopener noreferrer" style={styles.link}>
                Helpful Tips for Formatting <FaExternalLinkAlt />
              </a>
            </li>
            <li>
              <a href="https://mitpress.mit.edu/books/introduction-algorithms-third-edition" target="_blank" rel="noopener noreferrer" style={styles.link}>
                Introduction to Alogorithms <FaExternalLinkAlt />
              </a>
            </li>
          </ul>
        </div>

      </div>

    </footer>
  );
}

export default Footer
