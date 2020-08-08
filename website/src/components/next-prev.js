import React from 'react';
import { Link } from 'gatsby';

function NextPrev({ prev, prevUrl, next, nextUrl }) {
	return (
		<div className="NextPrev">
			{prev === undefined ? (
				<div />
			) : (
				<Link className="NextPrev-link" to={prevUrl}>
					<small>
						<strong>Previous:</strong>
					</small>
					<br />
					{prev}
				</Link>
			)}
			{next === undefined ? (
				<div />
			) : (
				<Link className="NextPrev-link" to={nextUrl}>
					<small>
						<strong>Next:</strong>
					</small>
					<br />
					{next}
				</Link>
			)}
		</div>
	);
}

export default NextPrev;
