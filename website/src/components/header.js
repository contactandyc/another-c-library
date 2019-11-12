import { Link } from "gatsby"
import React from "react"
import { FaExternalLinkAlt } from "react-icons/fa"

function Header() {
  const styles = {
    header: {
      position: `sticky`,
      top: `0`,
      backgroundColor: `#1A202C`,
      zIndex: `1`,
    },
    nav: {
      margin: `auto`,
      maxWidth: 1200,
    },
    link: {
      color: `white`,
      fontSize: `1.25rem`,
      padding: `1rem 1.5rem`,
      fontFamily: `-apple-system,'BlinkMacSystemFont','Segoe UI','Roboto','Oxygen','Ubuntu','Cantarell','Fira Sans','Droid Sans','Helvetica Neue',sans-serif`
   },
    linkActive: {
      color: `#38A169`,
      borderBottom: `solid 4px #2F855A`,
      backgroundColor: `#2D3748`
    }
  }

  return (
    <header style={styles.header}>
      <nav style={styles.nav}>
        <Link
          style={styles.link}
          activeStyle={styles.linkActive}
          to="/">
        <span style={{ fontWeight: `700` }}>Another C Library</span></Link>
        <Link
          style={styles.link}
          activeStyle={styles.linkActive}
          partiallyActive={true}
          to="/docs/">
          Documentation
        </Link>
        <Link
          style={styles.link}
          activeStyle={styles.linkActive}
          partiallyActive={true}
          to="/ebook/">
          A C eBook
        </Link>
        <a
          style={styles.link}
          target="_blank"
          rel="noopener noreferrer"
          href="https://github.com/contactandyc/another-c-library">
          Github <FaExternalLinkAlt style={{ paddingTop: 5 }}/>
        </a>
      </nav>
    </header>
  );
}

export default Header
