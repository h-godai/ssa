//
//  Shader.fsh
//  OpenGLES-test2
//
//  Created by IKEDA KOHEI on 11/04/01.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
