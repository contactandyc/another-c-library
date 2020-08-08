import React from 'react';
import PropTypes from 'prop-types';

function Repl({ url, height }) {
	const h = height ? `${height}px` : `400px`;

	return (
		<iframe
			title="Repl.it code"
			height={h}
			width="100%"
			src={`${url}?lite=true`}
			scrolling="no"
			frameBorder="no"
			allowFullScreen={true}
			sandbox="allow-forms allow-pointer-lock allow-popups allow-same-origin allow-scripts allow-modals"></iframe>
	);
}

Repl.propTypes = {
	url: PropTypes.string.isRequired,
	height: PropTypes.string.isRequired,
};

export default Repl;
