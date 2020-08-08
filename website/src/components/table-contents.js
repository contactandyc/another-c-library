import React from 'react';

function TableContents(props) {
	return (
		<div className="TableContents">
			{props.contents.map((el) => (
				<small key={el[1]}>
					<a className="TableContents-link" href={el[1]}>
						{el[0]}
					</a>
				</small>
			))}
		</div>
	);
}

export default TableContents;
