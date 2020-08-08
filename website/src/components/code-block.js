import React from 'react';
import Highlight, { defaultProps } from 'prism-react-renderer';
import github from 'prism-react-renderer/themes/github';

function CodeBlock(props) {
	const className = props.children.props.className || '';
	const matches = className.match(/language-(?<lang>.*)/);
	const styles = {
		margin: `0 0 1.45rem 0`,
		fontSize: `0.85rem`,
		lineHeight: `1.42`,
		background: `var(--codebg)`,
		borderRadius: `8px`,
		overflow: `auto`,
		wordWrap: `normal`,
		padding: `1.45rem`,
		fontFamily: `'SFMono-Regular', Consolas, 'Roboto Mono', 'Droid Sans Mono',
		'Liberation Mono', Menlo, Courier, monospace`,
	};

	return (
		<Highlight
			{...defaultProps}
			theme={github}
			code={props.children.props.children}
			language={
				matches && matches.groups && matches.groups.lang
					? matches.groups.lang
					: ''
			}>
			{({ className, style, tokens, getLineProps, getTokenProps }) => (
				<pre className={className} style={styles}>
					{tokens.map((line, i) => (
						<div {...getLineProps({ line, key: i })}>
							{line.map((token, key) => (
								<span {...getTokenProps({ token, key })} />
							))}
						</div>
					))}
				</pre>
			)}
		</Highlight>
	);
}

export default CodeBlock;
