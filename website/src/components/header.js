import { Link } from 'gatsby';
import PropTypes from 'prop-types';
import React, { useContext } from 'react';
import { ThemeToggler } from 'gatsby-plugin-dark-mode';

import { FiGithub, FiMenu, FiSun, FiMoon } from 'react-icons/fi';
import { MenuContext } from '../core/menu-context';

function Header({ siteTitle }) {
	const { menu, toggleMenu } = useContext(MenuContext);

	return (
		<header className="Header">
			<div>
				<Link className="Header-link" to="/">
					{siteTitle}
				</Link>
				<input
					aria-label="Search"
					className="Header-input"
					placeholder="Search"
				/>
			</div>
			<div className="Header-flex">
				<small className="Header-small">Beta</small>
				<a
					aria-label="GitHub"
					href="https://github.com/contactandyc/another-c-library"
					target="_blank"
					rel="noreferrer"
					className="Header-flex">
					<FiGithub className="Header-icon" />
				</a>
				<ThemeToggler type="button" tabIndex={0}>
					{({ theme, toggleTheme }) => (
						<div className="Header-flex">
							{theme === 'dark' ? (
								<button
									aria-label="Light mode"
									className="Header-btn"
									onClick={() => toggleTheme('light')}>
									<FiSun className="Header-icon" />
								</button>
							) : (
								<button
									aria-label="Dark mode"
									className="Header-btn"
									onClick={() => toggleTheme('dark')}>
									<FiMoon className="Header-icon" />
								</button>
							)}
						</div>
					)}
				</ThemeToggler>
				<button
					aria-label="Toggle menu"
					className="Header-btn"
					onClick={() => toggleMenu(!menu)}>
					<FiMenu className="Header-icon" />
				</button>
			</div>
		</header>
	);
}

Header.propTypes = {
	siteTitle: PropTypes.string,
};

Header.defaultProps = {
	siteTitle: ``,
};

export default Header;
