from fastapi import HTTPException, APIRouter, Query
from dotenv import load_dotenv
import os
import psycopg2

router = APIRouter()


def connect_to_database():
    load_dotenv()
    db_host = os.getenv('DATABASE_HOST')
    db_port = os.getenv('DATABASE_PORT')
    db_name = os.getenv('DATABASE_NAME')
    db_user = os.getenv('DATABASE_USER')
    db_password = os.getenv('DATABASE_PASSWORD')

    conn = psycopg2.connect(
        host=db_host,
        port=db_port,
        database=db_name,
        user=db_user,
        password=db_password
    )
    return conn


def format_micros(time_str):
    while True:
        if time_str[len(time_str) - 1] == '0':
            time_str = time_str[:-1]
        else:
            break
    return time_str


@router.get('/v3/users/{user_id}/badge_history')        #http://127.0.0.1:8000/v3/users/120/badge_history
def get_user_badge_history(user_id: int):
    try:
        conn = connect_to_database()
        cursor = conn.cursor()
        cursor.execute("""SELECT 
                                id,
                                title,
                                CASE
                                    WHEN type = 'aaapost' THEN 'post'
                                    WHEN type = 'badge' THEN 'badge'
                                END AS type,
                                TO_CHAR(created_at AT TIME ZONE 'UTC', 'YYYY-MM-DD"T"HH24:MI:SS.FF3+00') AS created_at,
                                position
                            FROM (
                                SELECT 
                                    id, 
                                    title,
                                    type,
                                    created_at,
                                    row_number() OVER(PARTITION BY type ORDER BY created_at) AS position
                                FROM (
                                    SELECT *
                                    FROM (
                                        SELECT	
                                            *,
                                            lag(type) OVER() AS type_before,
                                            lead(type) OVER() AS type_after
                                        FROM (
                                            SELECT 
                                                p.id AS id,
                                                p.title AS title,
                                                'aaapost' AS type,
                                                p.creationdate AS created_at
                                            FROM posts p
                                            WHERE owneruserid = %s
                                        
                                            UNION
                                            
                                            SELECT DISTINCT 
                                                b.id AS id,
                                                b.name AS title,
                                                'badge' AS type,
                                                b.date AS created_at
                                            FROM badges b
                                            WHERE b.userid = %s
                                            ORDER BY created_at
                                        ) AS sub1 
                                    ) AS sub2 
                                    WHERE type = 'badge' AND type_before = 'aaapost' OR 
                                          type = 'aaapost' AND type_after = 'badge'
                                    ORDER BY created_at
                                ) AS sub3 
                            ) AS sub4  
                            ORDER BY position;
        """, (user_id, user_id))

        user_activity = cursor.fetchall()
        cursor.close()
        conn.close()
        formatted_user_activity = []
        for activity in user_activity:
            formatted_user_activity.append({
                "id": activity[0],
                "title": activity[1],
                "type": activity[2],
                "created_at": activity[3],
                "position": activity[4]
            })
        response = {
            "items": formatted_user_activity
        }
        return response
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@router.get('/v3/tags/{tagname}/comments')      #http://127.0.0.1:8000/v3/tags/networking/comments?count=40
def get_tag_comments(tagname: str, count: int):
    try:
        conn = connect_to_database()
        cursor = conn.cursor()
        cursor.execute("""
            SELECT
                final.postid AS post_id,
                final.title AS title,
                final.displayname AS displayname,
                final.text AS text,
                TO_CHAR(final.post_created_at AT TIME ZONE 'UTC', 'YYYY-MM-DD"T"HH24:MI:SS.FF3+00') AS post_created_at,
                TO_CHAR(final.created_at AT TIME ZONE 'UTC', 'YYYY-MM-DD"T"HH24:MI:SS.FF3+00') AS created_at,
                TO_CHAR(final.diff, 'HH24:MI:SS.FF6') AS diff,
                TO_CHAR(AVG(final.diff) OVER (PARTITION BY final.postid ORDER BY final.created_at), 'HH24:MI:SS.FF6') AS avg
            FROM (
                SELECT *,
                    CASE 
                        WHEN data.previous IS NOT NULL THEN
                            CAST(data.created_at - data.previous AS INTERVAL) 
                        ELSE 
                            CAST(data.created_at - data.post_created_at AS INTERVAL)
                    END AS diff
                FROM (
                    SELECT 
                        p.id AS postid,
                        p.title AS title,
                        u.displayname AS displayname,
                        query_comms.text AS text,
                        p.creationdate AS post_created_at,
                        query_comms.creationdate AS created_at,
                        LAG(query_comms.creationdate) OVER(PARTITION BY p.id) AS previous
                    FROM (
                        SELECT 
                            c.*
                        FROM comments c 
                        WHERE c.postid IN (
                            SELECT DISTINCT
                                postid
                            FROM (
                                SELECT 
                                    c.*,
                                    ROW_NUMBER() OVER(PARTITION BY c.postid) AS comm_no
                                FROM comments c
                                WHERE c.postid IN (
                                    SELECT DISTINCT p.id
                                    FROM posts p 
                                    JOIN post_tags pt ON pt.post_id = p.id
                                    JOIN tags t ON t.id = pt.tag_id
                                    WHERE t.tagname = %s
                                )
                                ORDER BY c.creationdate
                            ) AS sub1
                            WHERE comm_no > %s
                        )
                        ORDER BY c.creationdate
                    ) AS query_comms
                    JOIN posts p ON p.id = postid
                    LEFT JOIN users u ON u.id = userid
                ) AS data
            ) AS final;
        """, (tagname, count))

        tag_comments = cursor.fetchall()
        formatted_tag_comments = []
        for comment in tag_comments:
            diff_str = format_micros(str(comment[6]))
            avg_str = format_micros(str(comment[7]))
            formatted_tag_comments.append({
                "post_id": comment[0],
                "title": comment[1],
                "displayname": comment[2],
                "text": comment[3],
                "post_created_at": comment[4],
                "created_at": comment[5],
                "diff": diff_str,
                "avg": avg_str
            })
        response = {
            "items": formatted_tag_comments
        }
        cursor.close()
        conn.close()

        return response
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@router.get('/v3/tags/{tagname}/comments/{position}')       #http://127.0.0.1:8000/v3/tags/linux/comments/2?limit=1
def get_tagged_post_comments_at_position(tagname: str, position: int, limit: int = Query(...)):
    try:
        conn = connect_to_database()
        cursor = conn.cursor()
        cursor.execute("""
            SELECT 
                querycomments.commentid AS id,
                u.displayname AS displayname,
                querycomments.body,
                querycomments.text,
                querycomments.score,
                querycomments.position
            FROM (
                SELECT 
                    *
                FROM (
                    SELECT 
                        c.id AS commentid,
                        c.userid AS userid,
                        oldest_posts.id AS postid,
                        oldest_posts.body AS body,    
                        c.text AS text,
                        c.score AS score,
                        row_number() OVER (PARTITION BY oldest_posts.id ORDER BY c.creationdate) AS position
                    FROM (
                        SELECT 
                            p.id,
                            p.creationdate,
                            p.body
                        FROM posts p
                        JOIN post_tags pt ON p.id = pt.post_id
                        JOIN tags t ON pt.tag_id = t.id
                        WHERE 
                            t.tagname = %s
                        ORDER BY
                            p.creationdate
                    ) AS oldest_posts
                    JOIN comments c ON c.postid = oldest_posts.id
                    ORDER BY oldest_posts.creationdate, c.creationdate
                ) AS sub1
                WHERE position = %s
                LIMIT %s
            ) AS querycomments
            JOIN users u ON u.id = querycomments.userid;
        """, (tagname, position, limit))

        tag_comments = cursor.fetchall()
        cursor.close()
        conn.close()
        formatted_tag_comments = []
        for comment in tag_comments:
            formatted_tag_comments.append({
                "id": comment[0],
                "displayname": comment[1],
                "body": comment[2],
                "text": comment[3],
                "score": comment[4],
                "position": comment[5]
            })
        response = {
            "items": formatted_tag_comments
        }
        return response
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@router.get('/v3/posts/{post_id}')      #http://127.0.0.1:8000/v3/posts/2154?limit=2
def get_post_thread(post_id: int, limit: int = Query(...)):
    try:
        conn = connect_to_database()
        cursor = conn.cursor()
        cursor.execute("""
            SELECT
                u.displayname,
                p.body,
                TO_CHAR(p.creationdate AT TIME ZONE 'UTC', 'YYYY-MM-DD"T"HH24:MI:SS.FF3+00') AS created_at
            FROM
                posts p 
            JOIN users u ON p.owneruserid = u.id
            WHERE
                p.id = %s
            UNION
            SELECT
                u.displayname,
                p.body,
                TO_CHAR(p.creationdate AT TIME ZONE 'UTC', 'YYYY-MM-DD"T"HH24:MI:SS.FF3+00') AS created_at
            FROM
                posts p
            JOIN users u ON p.owneruserid = u.id
            WHERE
                parentid = %s
            ORDER BY
                created_at ASC
            LIMIT %s;
        """, (post_id, post_id, limit))

        post_thread = cursor.fetchall()
        formatted_post_thread = []
        for post in post_thread:
            formatted_post_thread.append({
                "displayname": post[0],
                "body": post[1],
                "created_at": post[2]
            })
        cursor.close()
        conn.close()
        response = {
            "items": formatted_post_thread
        }
        return response
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))