import React from 'react';

function Footer() {
	return (
		<footer className="Footer">
			© {new Date().getFullYear()}. Built with{' '}
			<a className="Footer-a" href="https://www.gatsbyjs.org">
				Gatsby
			</a>{' '}
			by{' '}
			<a className="Footer-a" href="https://www.curtiscodes.com">
				Daniel Curtis
			</a>
			.
		</footer>
	);
}

export default Footer;
