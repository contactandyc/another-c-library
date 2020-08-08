import React from 'react';

function Heading({ tag, children }) {
	const Htag = `h${tag}`;

	return <Htag>{children}</Htag>;
}

export default Heading;
