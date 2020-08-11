import React from 'react';
import { Link } from 'gatsby';

function Footer() {
	return (
		<footer className="Footer">
			© {new Date().getFullYear()} Andy Curtis & Daniel Curtis.{' '}
			<Link to="/" className="Footer-a">
				Home
			</Link>{' '}
			|{' '}
			<Link to="/contact" className="Footer-a">
				Contact
			</Link>{' '}
			|{' '}
			<a
				className="Footer-a"
				href="https://www.github.com/contactandyc/another-c-library">
				GitHub
			</a>
		</footer>
	);
}

export default Footer;
