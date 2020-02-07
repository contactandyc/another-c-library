import { Link } from "gatsby";
import React, { useState } from "react";
import { FaExternalLinkAlt } from "react-icons/fa";

function Mobile({ open }) {
  let menuClass = open ? "Header-mobile-open" : "Header-mobile";

  return (
    <div className={menuClass}>
      <Link
        className="Header-a"
        activeClassName="Header-a-active"
        partiallyActive={true}
        to="/docs/"
        >
        Documentation
      </Link>
      <Link
        className="Header-a"
        activeClassName="Header-a-active"
        partiallyActive={true}
        to="/tutorial/"
        >
        Tutorial
      </Link>
      <a
        className="Header-a"
        target="_blank"
        rel="noopener noreferrer"
        href="https://github.com/contactandyc/another-c-library">
        Github <FaExternalLinkAlt style={{ paddingTop: 5 }}/>
      </a>
      <p className="Header-a">vBeta</p>
    </div>
  );
}

function Menu() {
  return (
    <div className="Header-menu">
      <Link
        className="Header-a"
        activeClassName="Header-a-active"
        partiallyActive={true}
        to="/docs/"
        >
        Documentation
      </Link>
      <Link
        className="Header-a"
        activeClassName="Header-a-active"
        partiallyActive={true}
        to="/tutorial/"
        >
        Tutorial
      </Link>
      <a
        className="Header-a"
        target="_blank"
        rel="noopener noreferrer"
        href="https://github.com/contactandyc/another-c-library">
        Github <FaExternalLinkAlt style={{ paddingTop: 5 }}/>
      </a>
      <p className="Header-a">vBeta</p>
    </div>
  );
}

function Burger({ open, setOpen }) {
  let burgerClass = open ? "Header-burger Header-burger-open" : "Header-burger";

  return (
    <div className={burgerClass} onClick={() => setOpen(!open)}>
      <div />
      <div />
      <div />
    </div>
  );
}

function Header() {
  const [open, setOpen] = useState(false);

  return (
    <header className="Header">
      <nav>
        <h1><Link to="/">Another C Library</Link></h1>
        <div>
          <Menu />
          <Burger open={open} setOpen={setOpen} />
          <Mobile open={open} />
        </div>
      </nav>
    </header>
  );
}

export default Header;
