export default class MoveFunctions {
    outOfBounds(Render, shp) {
        for (let x=0; x<shp.length; x++)
            for (let y=0; y<shp[0].length; y++) {
                if (!shp[x][y])
                    continue;
                let out_of_bounds=Render.pos[0]+x>=Render.rows||Render.pos[0]+x<0;
                if (!out_of_bounds&&Render.board[Render.pos[0]+x][Render.pos[1]+y]==2)
                    return true;
                if (out_of_bounds)
                    return true;
            }
        return false;
    }
    outOfFloor(Render, shp) {
        for (let x=0; x<shp.length; x++)
            for (let y=0; y<shp[0].length; y++) {
                if (!shp[x][y])
                    continue;
                if (Render.pos[1]+y>=Render.cols)
                    return true;
                if (Render.board[Render.pos[0]+x][Render.pos[1]+y]==2)
                    return true;
            }
        return false;
    }
    transpose(shp) {
        let outshp=[];
        for (let y=-1, x=[]; ++y<shp.length; x=[])
            for (outshp.push(x); x.length<shp[0].length; x.push(shp[x.length][y]));
        return outshp;
    }
    reflect(shp) {
        let outshp=[];
        for (let i=-1; ++i<shp.length; outshp.push(shp[i].reverse()));
        return outshp;
    }
    rotateShp(Render, turns) {
        let outshp=Render.getShape();
        for (let i=turns; i-->0; outshp=this.reflect(outshp))
            outshp=this.transpose(outshp);
        return outshp;
    }
    rotate(Render, turns) {
        let outshp=this.rotateShp(Render, Render.current_shape_rotation+turns);
        if (this.outOfBounds(Render, outshp))
            return;
        Render.current_shape_rotation+=turns;
    }
    reset(Render, old_pos, old) {
        Render.pos=old_pos;
        Render.board=old;
    }
    strafe(Render, drt) {
        let old_pos=Render.pos,
            old=Render.board,
            rot_shp=this.rotateShp(Render, Render.current_shape_rotation);
        Render.pos=[Render.pos[0]+drt[0], Render.pos[1]+drt[1]];
        if (drt[0]&&this.outOfBounds(Render, rot_shp))
            return this.reset(Render, old_pos, old);
        if (!this.outOfFloor(Render, rot_shp))
            return;
        Render.nextShape()
    }
}