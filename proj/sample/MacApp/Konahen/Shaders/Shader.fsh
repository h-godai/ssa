//
//  Shader.fsh
//  Konahen
//
//  Created by nishi on 11/09/21.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
