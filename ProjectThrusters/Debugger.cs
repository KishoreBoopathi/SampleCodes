using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Debugger : MonoBehaviour
{
    bool collisionDisabled = false;
    // Update is called once per frame
    void Update()
    {
        if(Input.GetKeyDown(KeyCode.L))
            TraverseToNextLevel();

        if(Input.GetKeyDown(KeyCode.C))
            ToggleCollisionsStatus();
    }

    void TraverseToNextLevel()
    {
        int currentSceneIndex = SceneManager.GetActiveScene().buildIndex;
        int nextSceneIndex = currentSceneIndex + 1;
        if(nextSceneIndex == SceneManager.sceneCountInBuildSettings)
        {
            Application.Quit();
        }
        SceneManager.LoadScene(nextSceneIndex);
    }

    void ToggleCollisionsStatus()
    {
        collisionDisabled = !collisionDisabled;
    }

    public bool GetCollisionStatus()
    {
        return collisionDisabled;
    }

}
