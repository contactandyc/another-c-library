import React from "react"
import { graphql, useStaticQuery, Link } from "gatsby"
import { FaExternalLinkAlt } from "react-icons/fa"

function Footer() {
  const data = useStaticQuery(graphql`
    query SitePages {
      allMarkdownRemark(sort: {order: ASC, fields: [frontmatter___title]}) {
        edges {
          node {
            id
            frontmatter {
              title
              posttype
              path
            }
          }
        }
      }
    }
  `);

  const docs = data.allMarkdownRemark.edges
                .filter(i => i.node.frontmatter.title.length > 0)
                .filter(i => i.node.frontmatter.posttype === "docs");

  const tuts = data.allMarkdownRemark.edges
                .filter(elm => elm.node.frontmatter.title.length > 0)
                .filter(elm => elm.node.frontmatter.posttype === "tutorial")
                .sort(function(a, b) {
                  let ai = parseInt(a.node.frontmatter.title);
                  let bi = parseInt(b.node.frontmatter.title);
                  return ai - bi;
                });

  return (
    <footer className="Footer-ctr">
      <div className="Footer">
        <div>
          <h1 style={{ margin: `10px 0` }}>Another C Library</h1>
          <p className="Footer-p">
            © {new Date().getFullYear()} Andy Curtis & Daniel Curtis
          </p>
          <ul className="Footer-ul">
            <li><Link className="Footer-lnk" to="/contact">
              Contact
            </Link></li>
            <li><Link className="Footer-lnk" to="/conduct">
              Code of Conduct
            </Link></li>
            <li><Link className="Footer-lnk" to="/contributing">
              Contributing
            </Link></li>
            <li><Link className="Footer-lnk" to="/license">
              License
            </Link></li>
          </ul>
        </div>

        <div>
          <div>
            <h2 style={{ margin: `10px auto` }}>Documentation</h2>
            <ul className="Footer-ul">
              {docs.map((val, i) => {
                  return (
                    <li key={i}>
                      <Link
                        to={`/docs${val.node.frontmatter.path}`}
                        style={{ color: "white" }}
                        >
                        {val.node.frontmatter.title}
                      </Link>
                    </li>
                  );
                })}
            </ul>
          </div>

          <h2 style={{ margin: `10px auto` }}>Resources</h2>
          <ul className="Footer-ul">
            <li>
              <a
                href="https://medium.com/software-design/why-software-developers-should-care-about-cpu-caches-8da04355bb8a"
                target="_blank"
                rel="noopener noreferrer"
                className="Footer-lnk"
                >
                Why software developers should care about CPU caches
                <FaExternalLinkAlt />
              </a>
            </li>
            <li>
              <a
                href="https://mitpress.mit.edu/books/introduction-algorithms-third-edition"
                target="_blank"
                rel="noopener noreferrer"
                className="Footer-lnk"
                >
                Introduction to Alogorithms <FaExternalLinkAlt />
              </a>
            </li>
          </ul>
        </div>

        <div>
          <h2 style={{ margin: `10px auto` }}>Tutorial</h2>
          <ul className="Footer-ul">
            {tuts.map((val, i) => {
                return (
                  <li key={i}>
                    <Link
                      to={`/tutorial${val.node.frontmatter.path}`}
                      style={{ color: "white" }}
                      >
                      {val.node.frontmatter.title}
                    </Link>
                  </li>
                );
              })}
          </ul>
        </div>

      </div>

    </footer>
  );
}

export default Footer;
