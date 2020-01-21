import { Link } from "gatsby"
import React from "react"
import { FaExternalLinkAlt } from "react-icons/fa"

function Header() {
  return (
    <header className="Header">
      <nav className="Header-nv">
        <Link
          className="Header-a"
          activeClassName="Header-a-actv"
          to="/">
        <span style={{ fontWeight: `700` }}>Another C Library</span></Link>
        <Link
          className="Header-a"
          activeClassName="Header-a-actv"
          partiallyActive={true}
          to="/docs/">
          Documentation
        </Link>
        <Link
          className="Header-a"
          activeClassName="Header-a-actv"
          partiallyActive={true}
          to="/tutorial/">
          Tutorial
        </Link>
        <a
          className="Header-a"
          target="_blank"
          rel="noopener noreferrer"
          href="https://github.com/contactandyc/another-c-library">
          Github <FaExternalLinkAlt style={{ paddingTop: 5 }}/>
        </a>
        <p className="Header-p">Beta version</p>
      </nav>
    </header>
  );
}

export default Header
