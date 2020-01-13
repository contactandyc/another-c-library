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

  data.allMarkdownRemark.edges.sort(function(a, b) {
    let ai = parseInt(a.node.frontmatter.title);
    let bi = parseInt(b.node.frontmatter.title);
    return ai - bi;
  });

  const elmArr = data.allMarkdownRemark.edges.filter(elm => elm.node.frontmatter.title.length > 0);
  const docsArr = elmArr.filter(elm => elm.node.frontmatter.posttype === "docs");
  const tutArr = elmArr.filter(elm => elm.node.frontmatter.posttype === "tutorial");

  return (
    <footer className="Footer-ctr">
      <div className="Footer">
        <div>
          <h1 style={{ margin: `10px 0` }}>Another C Library</h1>
          <p className="Footer-p">© {new Date().getFullYear()} Andy Curtis & Daniel Curtis</p>
          <ul className="Footer-ul">
            <li><Link className="Footer-lnk" to="/contact">Contact</Link></li>
            <li><Link className="Footer-lnk" to="/contributing">Contributing</Link></li>
            <li><Link className="Footer-lnk" to="/license">License</Link></li>
          </ul>
        </div>

        <div>
          <div>
            <h2 style={{ margin: `10px auto` }}>Documentation</h2>
            <ul className="Footer-ul">
              {docsArr.map((val, i) => {
                  return (
                    <li key={i}>
                      <Link to={`/docs${val.node.frontmatter.path}`}>
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
                className="Footer-lnk">
                  Why software developers should care about CPU caches <FaExternalLinkAlt />
              </a>
            </li>
            <li>
              <a
                href="https://github.com/contactandyc/another-c-library/blob/master/docs/tips.md#create-multiline-macro-in-c"
                target="_blank"
                rel="noopener noreferrer"
                className="Footer-lnk">
                  Helpful Tips for Formatting <FaExternalLinkAlt />
              </a>
            </li>
            <li>
              <a
                href="https://mitpress.mit.edu/books/introduction-algorithms-third-edition"
                target="_blank"
                rel="noopener noreferrer"
                className="Footer-lnk">
                  Introduction to Alogorithms <FaExternalLinkAlt />
              </a>
            </li>
          </ul>
        </div>

        <div>
          <h2 style={{ margin: `10px auto` }}>Tutorial</h2>
          <ul className="Footer-ul">
            {tutArr.map((val, i) => {
                return (
                  <li key={i}>
                    <Link to={`/tutorial${val.node.frontmatter.path}`}>
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
